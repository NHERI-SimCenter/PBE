#include "HDF5Handler.h"

#include <string>
#include <QString>

HDF5Data::HDF5Data(QString HDF5FilePath_)
	: HDF5FilePath(HDF5FilePath_)
{
    qDebug() << "Initializing file: " << HDF5FilePath;

    // open the file
    std::string strFilePath = HDF5FilePath.toStdString();
    HDF5File = H5Fopen(strFilePath.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    if (HDF5File < 0)
    {
        qDebug() << "Unable to open file: " << HDF5FilePath << endl;
        return;
    }

    // load the root group
    HDF5Root = H5Gopen(HDF5File, "/", H5P_DEFAULT);

};

HDF5Data::~HDF5Data() {	

	H5Gclose(HDF5Root);

    H5Fclose(HDF5File);

	qDebug() << "Releasing file: " << HDF5FilePath;
};

hid_t
HDF5Data::getMember(QString memberName) {

    hid_t member = H5Gopen(HDF5Root, memberName.toStdString().c_str(), H5P_DEFAULT);

	return member;
}

void
HDF5Data::describeGroup(hid_t gid) {

	// get information aobut the group name
    char group_name[MAX_NAME];
    ssize_t len;

    len = H5Iget_name(gid, group_name, MAX_NAME);

    qDebug() << "Group Name: " << group_name << endl;

    // scan the attributes of the group

    int na;
    hid_t aid;
    int i;
    char buf[MAX_NAME];
    ssize_t a_len;

    na = H5Aget_num_attrs(gid);

    for (i = 0; i < na; i++) {
    	aid = H5Aopen_idx(gid, (unsigned int)i );

    	a_len = H5Aget_name(aid, MAX_NAME, buf );
    	qDebug() << "\tAttribute Name: " << buf;

    	H5Aclose(aid);
    }

    // now get the members

    herr_t err;
    hsize_t nobj;
    ssize_t m_len;
    int m_type;
    char memb_name[MAX_NAME];
    //hid_t subgid;

    err = H5Gget_num_objs(gid, &nobj);
    for (hsize_t i = 0; i < nobj; i++) {

    	// get the member name
    	m_len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, 
    		                          (size_t)MAX_NAME);    	

    	// get the member type
    	m_type = H5Gget_objtype_by_idx(gid, (size_t)i);

    	switch(m_type) {
    		case H5G_LINK:
    			qDebug() << "H5G_LINK";
    			break;
    		case H5G_GROUP:
		  		qDebug() << endl << endl << group_name << "Member " << i << ": " << m_len << "\t" << memb_name << "H5G_GROUP" << endl;
                //subgid = getMember(memb_name);
                //describeGroup(subgid);
                //H5Gclose(subgid);
    			break;
    		case H5G_DATASET:
    			qDebug() << endl << endl << group_name << "Member " << i << ": " << m_len << "\t" << memb_name << "H5G_DATASET" << endl;
    			break;
    		case H5G_TYPE:
    			qDebug() << "H5G_TYPE";
    			break;
    		default:
    			qDebug() << "Unknown type";
    			break;
    	}
    }
}

void
HDF5Data::getDFIndexValues(hid_t gid) {

    int i;

    // load the index column from the data
	char index_name[MAX_NAME] = "axis1";
	char  **stringData; // Read buffer
	int dSize;

	getStringDataSet(gid, index_name, &stringData, &dSize);

	//for (i=0; i<dims[0]; i++) {
	for (i=0; i<10; i++) {
        qDebug() << i << QString(stringData[i]);
	} 

	free (stringData[0]);
	free (stringData);
}

void
HDF5Data::getStringDataSet(hid_t gid, char dsName[MAX_NAME], char ***udata, 
                           int *dataSize, int dimCount){

	// open the dataset
    hid_t dset = H5Dopen(gid, dsName, H5P_DEFAULT);

	// get the datatype
	hid_t dtype = H5Dget_type(dset);
    size_t dataDim = H5Tget_size(dtype);
	dataDim++;                              // Make room for null terminator

    //qDebug() << dtype << dataDim;

	// get the shape of the data
    hsize_t dims[2];
	int     ndims;
	hid_t   space;

	space = H5Dget_space(dset);
	ndims = H5Sget_simple_extent_dims(space, dims, NULL);

	//qDebug() << ndims << dims[0];
	//qDebug() << "Number of string entries: " << dims[0];

	// Allocate array of pointers to rows
	int   i;
    char **rdata;

    rdata = (char **) malloc (dims[0] * sizeof(char *));

    if (dimCount == 1){
    	rdata[0] = (char *) malloc (dims[0] * dataDim * sizeof(char));
    }
   	else if (dimCount == 2) {
   		rdata[0] = (char *) malloc (dims[0] * dims[1] * dataDim * sizeof(char));
   	}

    for (hsize_t i=1; i<dims[0]; i++) {
    	if (dimCount == 1) {
        	rdata[i] = rdata[0] + i * dataDim;
        } else if (dimCount == 2) {
        	rdata[i] = rdata[0] + i * dataDim * dims[1];
        }
    }	

	// Create the memory datatype
	herr_t status;
	hid_t  memtype;

	memtype = H5Tcopy(H5T_C_S1);
	status = H5Tset_size(memtype, dataDim);

	// Read the data
    status = H5Dread(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata[0]);

    *udata = rdata;
    *dataSize = dims[0];

    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (dtype);
    status = H5Tclose (memtype);
}

void
HDF5Data::getIntegerDataSet(hid_t gid, char dsName[MAX_NAME], int ***udata, 
                           int *dataSize, int dimCount){

	// open the dataset
    hid_t dset = H5Dopen(gid, dsName, H5P_DEFAULT);

	// get the shape of the data
    hsize_t dims[2];
	int     ndims;
	hid_t   space;

	space = H5Dget_space(dset);
	ndims = H5Sget_simple_extent_dims(space, dims, NULL);

	//qDebug() << ndims << dims[0];
	//qDebug() << "Number of string entries: " << dims[0];

	// Allocate array of pointers to rows
	int   i;
    int **rdata;

    rdata = (int **) malloc (dims[0] * sizeof(int *));
    if (dimCount == 1){
    	rdata[0] = (int *) malloc (dims[0] * sizeof(int));
    }
   	else if (dimCount == 2) {
   		rdata[0] = (int *) malloc (dims[0] * dims[1] * sizeof(int));
   	}

    for (i=1; i<dims[0]; i++) {
    	if (dimCount == 1) {
        	rdata[i] = rdata[0] + i;
        } else if (dimCount == 2) {
        	rdata[i] = rdata[0] + i * dims[1];
        }
    }	

	// Read the data
    herr_t status;
    status = H5Dread(dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
    				 rdata[0]);

    *udata = rdata;
    *dataSize = dims[0];

    status = H5Dclose (dset);
    status = H5Sclose (space);
}

void
HDF5Data::getFloatDataSet(hid_t gid, char dsName[MAX_NAME], double ***udata, 
                           int *dataSize, int dimCount){

	// open the dataset
    hid_t dset = H5Dopen(gid, dsName, H5P_DEFAULT);

	// get the shape of the data
    hsize_t dims[2];
	int     ndims;
	hid_t   space;

	space = H5Dget_space(dset);
	ndims = H5Sget_simple_extent_dims(space, dims, NULL);

	//qDebug() << ndims << dims[0];
	//qDebug() << "Number of string entries: " << dims[0];

	// Allocate array of pointers to rows
	int   i;
    double **rdata;

    rdata = (double **) malloc (dims[0] * sizeof(double *));
    if (dimCount == 1){
    	rdata[0] = (double *) malloc (dims[0] * sizeof(double));
    }
   	else if (dimCount == 2) {
   		rdata[0] = (double *) malloc (dims[0] * dims[1] * sizeof(double));
   	}

    for (i=1; i<dims[0]; i++) {
    	if (dimCount == 1) {
        	rdata[i] = rdata[0] + i;
        } else if (dimCount == 2) {
        	rdata[i] = rdata[0] + i * dims[1];
        }
    }	

	// Read the data
    herr_t status;
    status = H5Dread(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
    				 rdata[0]);

    *udata = rdata;
    *dataSize = dims[0];

    status = H5Dclose (dset);
    status = H5Sclose (space);
}

void
HDF5Data::getBoolDataSet(hid_t gid, char dsName[MAX_NAME], unsigned char ***udata, 
                           int *dataSize, int dimCount){

	// open the dataset
    hid_t dset = H5Dopen(gid, dsName, H5P_DEFAULT);

	// get the shape of the data
    hsize_t dims[2];
	int     ndims;
	hid_t   space;

	space = H5Dget_space(dset);
	ndims = H5Sget_simple_extent_dims(space, dims, NULL);

	//qDebug() << ndims << dims[0];
	//qDebug() << "Number of string entries: " << dims[0];

	// Allocate array of pointers to rows
	int   i;
    unsigned char **rdata;

    rdata = (unsigned char **) malloc (dims[0] * sizeof(unsigned char *));
    if (dimCount == 1){
    	rdata[0] = (unsigned char *) malloc (dims[0] * sizeof(unsigned char));
    }
   	else if (dimCount == 2) {
   		rdata[0] = (unsigned char *) malloc (dims[0] * dims[1] * sizeof(unsigned char));
   	}

    for (i=1; i<dims[0]; i++) {
    	if (dimCount == 1) {
        	rdata[i] = rdata[0] + i;
        } else if (dimCount == 2) {
        	rdata[i] = rdata[0] + i * dims[1];
        }
    }	

	// Read the data
    herr_t status;
    status = H5Dread(dset, H5T_NATIVE_B8, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
    				 rdata[0]);

    *udata = rdata;
    *dataSize = dims[0];

    status = H5Dclose (dset);
    status = H5Sclose (space);
}

void
HDF5Data::getDFEntry(hid_t gid, QString ds_name, QVector<int> *qvRes) {

	int **intData;
    char blockName[MAX_NAME];
	int dSize, row;
	QVector<int> results;

    //strcpy_s(blockName, ds_name.toStdString().c_str());
    strcpy(blockName, ds_name.toStdString().c_str());

    getIntegerDataSet(gid, blockName, &intData, &dSize);

    for (row=0; row<dSize; row++) {
        results.append(*intData[row]);
    }

    *qvRes = results;

    free (intData[0]);
    free (intData);
}

void
HDF5Data::getDFEntry(hid_t gid, QString ds_name, QVector<QString> *qvRes) {

    char **stringData;
    char blockName[MAX_NAME];
	int dSize, row;
	QVector<QString> results;

    strcpy(blockName, ds_name.toStdString().c_str());

    getStringDataSet(gid, blockName, &stringData, &dSize);

    for (row=0; row<dSize; row++) {
        results.append(stringData[row]);
    }

    *qvRes = results;

    free (stringData[0]);
    free (stringData);
}

void
HDF5Data::getDFEntry(hid_t gid, QString ds_name, QVector<double> *qvRes) {

    double **floatData;
    char blockName[MAX_NAME];
	int dSize, row;
	QVector<double> results;

    strcpy(blockName, ds_name.toStdString().c_str());

    getFloatDataSet(gid, blockName, &floatData, &dSize);

    for (row=0; row<dSize; row++) {
        results.append(*floatData[row]);
    }

    *qvRes = results;

    free (floatData[0]);
    free (floatData);
}

void
HDF5Data::getDFEntryLocation(hid_t gid, QString rowName, QStringList columnNames,
	                         int *blockID_out, int *colID_out){
	// Start by getting the number of blocks and the number of levels in the 
	// column header

	int *numLvls, *numBlocks;
	hid_t attr;

	attr = H5Aopen(gid, "axis0_nlevels", H5P_DEFAULT);
	numLvls = (int *) malloc (sizeof(int));
	H5Aread(attr, H5T_NATIVE_INT, numLvls);
	H5Aclose(attr);

	attr = H5Aopen(gid, "nblocks", H5P_DEFAULT);
	numBlocks = (int *) malloc (sizeof(int));
	H5Aread(attr, H5T_NATIVE_INT, numBlocks);
	H5Aclose(attr);

	qDebug() << *numLvls;
	qDebug() << *numBlocks;

    // Travel the block labels and search for the provided columnNames
    int i, blockID, colLvl, blockColID, dSize;
    char  **stringData; // Read buffer
    int colCount = columnNames.length();
    QVector<int> colLvlIDs;
    bool foundIt = false;
    std::string blockNameStr;
    char blockName[MAX_NAME];

    for (i=0; i<colCount; i++) {
    	colLvlIDs.append(-1);
    }

    blockID = 0; // because the listed block labels are the same for all blocks
    for (colLvl=0; colLvl<colCount; colLvl++) {

		qDebug() << columnNames[colLvl];

        blockNameStr = "block" + std::to_string(blockID) + "_items_level" + std::to_string(colLvl);
        strcpy(blockName, blockNameStr.c_str());

        getStringDataSet(gid, blockName, &stringData, &dSize);

		for (blockColID=0; blockColID<dSize; blockColID++) {

			if (stringData[blockColID] == columnNames[colLvl]) {
				qDebug() << blockColID << QString(stringData[blockColID]);
				colLvlIDs[colLvl] = blockColID;
				break;
			}		        
		} 

		free (stringData[0]);
		free (stringData);

		if (blockColID == dSize) {
			break;
		}
	}

    qDebug() << colLvlIDs;

    // Now get the column IDs in the database following the label ids
    QVector<int> colIDs;
    int  **intData; // Read buffer

    for (i=0; i<colCount; i++) {
    	colIDs.append(-1);
    }

    for (blockID = 0; blockID < *numBlocks; blockID++) {

        qDebug() << "block " << blockID;

        blockNameStr = "block" + std::to_string(blockID) + "_items_label" + std::to_string(0);
        strcpy(blockName, blockNameStr.c_str());

        getIntegerDataSet(gid, blockName, &intData, &dSize);

        free (intData[0]);
        free (intData);

        for (blockColID=0; blockColID<dSize; blockColID++) {

            for (colLvl=0; colLvl<colCount; colLvl++) {

                blockNameStr = "block" + std::to_string(blockID) + "_items_label" + std::to_string(colLvl);
                strcpy(blockName, blockNameStr.c_str());

                getIntegerDataSet(gid, blockName, &intData, &dSize);

                if (*intData[blockColID] == colLvlIDs[colLvl]) {

                    qDebug() << blockColID << *intData[blockColID];

                    colIDs[colLvl] = blockColID;
                    continue;
                }
                break;
			} 

			free (intData[0]);
			free (intData);

            if (colIDs[colCount-1] != -1) {
                break;
            }
    	}

        foundIt = (colIDs[0] != -1);
        for (i=1; i<colCount; i++) {
            foundIt = (foundIt && (colIDs[i] != -1));
	    }

        if (foundIt == true) {
        	qDebug() << "FOUND IT!" << colIDs;
	    	break;
        } else {
            qDebug() << "HAVE NOT FOUND IT YET!" << colIDs;
        }
    }

    qDebug() << "Done" << blockID << colIDs;

    *blockID_out = blockID;
    *colID_out = colIDs[0];

    free (numLvls);
    free (numBlocks);
}
