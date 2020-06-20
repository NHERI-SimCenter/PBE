#ifndef HDF5HANDLER_H
#define HDF5HANDLER_H

#include <hdf5.h>
#include <QString>
#include <QDebug>
#include <QVector>

#define MAX_NAME 1024

class HDF5Data {

public:
	HDF5Data(QString HDF5FilePath_);
	~HDF5Data();

	void describeGroup(hid_t gid);	
    void getDFIndexValues(hid_t gid);
    void getDFEntry(hid_t gid, QString ds_name, QVector<int> *qvRes);
    void getDFEntry(hid_t gid, QString ds_name, QVector<QString> *qvRes);
    void getDFEntry(hid_t gid, QString ds_name, QVector<double> *qvRes);

	hid_t getMember(QString memberNameStr);
	void getStringDataSet(hid_t gid, char dsName[MAX_NAME], char ***udata,
		                  int *dataSize, int dimCount = 1);
	void getIntegerDataSet(hid_t gid, char dsName[MAX_NAME], int ***udata,
		                  int *dataSize, int dimCount = 1);
	void getBoolDataSet(hid_t gid, char dsName[MAX_NAME], unsigned char ***udata, 
						   int *dataSize, int dimCount = 1);
	void getFloatDataSet(hid_t gid, char dsName[MAX_NAME], double ***udata,
		                  int *dataSize, int dimCount = 1);
	void getDFEntryLocation(hid_t gid, QString rowName, QStringList columnNames,
							int *blockID_out, int *colID_out);

private:
	QString HDF5FilePath;
	hid_t HDF5File;
	hid_t HDF5Root;
};

#endif // HDF5HANDLER_H
