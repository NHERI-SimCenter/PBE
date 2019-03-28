import sys
import os
import numpy as np
import json

import pelicunPBE
from pelicunPBE.control import FEMA_P58_Assessment, HAZUS_Assessment
from pelicunPBE.file_io import write_SimCenter_DL_output

def replace_FG_IDs_with_FG_names(assessment, df):
	FG_list = sorted(assessment._FG_dict.keys())
	new_col_names = dict(
		(fg_id, fg_name) for (fg_id, fg_name) in 
		zip(np.arange(1, len(FG_list) + 1), FG_list))

	return df.rename(columns=new_col_names)

def run_pelicun(DL_input_path, EDP_input_path, 
	CMP_data_path=None, POP_data_path=None):

	target_path = DL_input_path[:-len('dakota.json')]
	#print(target_path)

	# delete output files from previous runs (if needed)
	files_to_delete = [
		'DL_summary.csv',
        'DL_summary_stats.csv',
        'DMG.csv',
        'DMG_agg.csv',
        'DV_red_tag.csv',
        'DV_red_tag_agg.csv',
        'DV_rec_cost.csv',
        'DV_rec_cost_agg.csv',
        'DV_rec_time.csv',
        'DV_rec_time_agg.csv',
        'DV_injuries_0.csv',
        'DV_injuries_0_agg.csv',
        'DV_injuries_1.csv',
        'DV_injuries_1_agg.csv',
	]
	for file_name in files_to_delete:
		try:
			os.remove(target_path+file_name)
		except:
			pass

	# read the type of assessment from the DL input file
	with open(DL_input_path, 'r') as f:
		DL_input = json.load(f)

	DL_method = DL_input['LossModel']['DLMethod']

	if DL_method == 'FEMA P58':
		A = FEMA_P58_Assessment()
	elif DL_method == 'HAZUS MH':
		A = HAZUS_Assessment()

	A.read_inputs(DL_input_path, EDP_input_path, 
		CMP_data_path, POP_data_path, verbose=False)

	A.define_random_variables()

	A.define_loss_model()

	A.calculate_damage()

	A.calculate_losses()

	A.aggregate_results()

	try:
		write_SimCenter_DL_output(target_path+'DL_summary.csv', A._SUMMARY, 
			index_name='#Num', collapse_columns=True)

		write_SimCenter_DL_output(target_path+'DL_summary_stats.csv', A._SUMMARY, 
			index_name='attribute',
			collapse_columns=True, stats_only=True)

		EDPs = sorted(A._EDP_dict.keys())
		write_SimCenter_DL_output(
			target_path+'EDP.csv', A._EDP_dict[EDPs[0]]._RV.samples,
			index_name='#Num', collapse_columns=False)
		
		DMG_mod = replace_FG_IDs_with_FG_names(A, A._DMG)
		write_SimCenter_DL_output(
			target_path+'DMG.csv', DMG_mod,
			index_name='#Num', collapse_columns=False)

		write_SimCenter_DL_output(
			target_path+'DMG_agg.csv', DMG_mod.T.groupby(level=0).aggregate(np.sum).T,
			index_name='#Num', collapse_columns=False)

		DV_mods, DV_names = [], []
		for key in A._DV_dict.keys():
			if key != 'injuries':
				DV_mods.append(replace_FG_IDs_with_FG_names(A, A._DV_dict[key]))
				DV_names.append('DV_{}'.format(key))
			else:
				for i in range(2 if DL_method == 'FEMA P58' else 4):
					DV_mods.append(replace_FG_IDs_with_FG_names(A, A._DV_dict[key][i]))
					DV_names.append('DV_{}_{}'.format(key, i))

		for DV_mod, DV_name in zip(DV_mods, DV_names):
			write_SimCenter_DL_output(
			target_path+DV_name+'.csv', DV_mod, index_name='#Num', collapse_columns=False)

			write_SimCenter_DL_output(
			target_path+DV_name+'_agg.csv', DV_mod.T.groupby(level=0).aggregate(np.sum).T,
			index_name='#Num', collapse_columns=False)

	except:
		print("ERROR when trying to create DL output files.")

	return 0

if __name__ == '__main__':

	if sys.argv[3] not in [None, 'None']:
		CMP_data_path = sys.argv[3]+'/'
	else:
		CMP_data_path = None

	if sys.argv[4] not in [None, 'None']:
		POP_data_path = sys.argv[4]
	else:
		POP_data_path = None

	sys.exit(
		run_pelicun(sys.argv[1], sys.argv[2], CMP_data_path, POP_data_path))