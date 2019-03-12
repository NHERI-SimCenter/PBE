import sys
import os
import numpy as np

import pelicunPBE
from pelicunPBE.control import FEMA_P58_Assessment
from pelicunPBE.file_io import write_SimCenter_DL_output

def replace_FG_IDs_with_FG_names(assessment, df):
	FG_list = sorted(assessment._FG_dict.keys())
	new_col_names = dict(
		(fg_id, fg_name) for (fg_id, fg_name) in 
		zip(np.arange(1, len(FG_list) + 1), FG_list))

	return df.rename(columns=new_col_names)

def run_pelicun(DL_input_path, EDP_input_path, 
	CMP_data_path=None, POP_data_path=None):

	# delete output files from previous runs (if needed)
	try:
		os.remove('DL_summary.csv')
		os.remove('DL_summary_stats.csv')
		os.remove('DMG.csv')
		os.remove('DMG_agg.csv')
		os.remove('DV_red_tag.csv')
		os.remove('DV_red_tag_agg.csv')
		os.remove('DV_rec_cost.csv')
		os.remove('DV_rec_cost_agg.csv')
		os.remove('DV_rec_time.csv')
		os.remove('DV_rec_time_agg.csv')
		os.remove('DV_injuries_0.csv')
		os.remove('DV_injuries_0_agg.csv')
		os.remove('DV_injuries_1.csv')
		os.remove('DV_injuries_1_agg.csv')
	except:
		pass

	A = FEMA_P58_Assessment()
	A.read_inputs(DL_input_path, EDP_input_path, 
		CMP_data_path, POP_data_path, verbose=False)

	A.define_random_variables()

	A.define_loss_model()

	A.calculate_damage()

	A.calculate_losses()

	A.aggregate_results()

	write_SimCenter_DL_output('DL_summary.csv', A._SUMMARY, 
		index_name='#Num', collapse_columns=True)

	write_SimCenter_DL_output('DL_summary_stats.csv', A._SUMMARY, 
		index_name='attribute',
		collapse_columns=True, stats_only=True)

	EDPs = sorted(A._EDP_dict.keys())
	write_SimCenter_DL_output(
		'EDP.csv', A._EDP_dict[EDPs[0]]._RV.samples,
		index_name='#Num', collapse_columns=False)
	
	DMG_mod = replace_FG_IDs_with_FG_names(A, A._DMG)
	write_SimCenter_DL_output(
		'DMG.csv', DMG_mod,
		index_name='#Num', collapse_columns=False)

	write_SimCenter_DL_output(
		'DMG_agg.csv', DMG_mod.T.groupby(level=0).aggregate(np.sum).T,
		index_name='#Num', collapse_columns=False)

	DV_mods, DV_names = [], []
	for key in A._DV_dict.keys():
		if key != 'injuries':
			DV_mods.append(replace_FG_IDs_with_FG_names(A, A._DV_dict[key]))
			DV_names.append('DV_{}'.format(key))
		else:
			for i in range(2):
				DV_mods.append(replace_FG_IDs_with_FG_names(A, A._DV_dict[key][i]))
				DV_names.append('DV_{}_{}'.format(key, i))

	for DV_mod, DV_name in zip(DV_mods, DV_names):
		write_SimCenter_DL_output(
		DV_name+'.csv', DV_mod, index_name='#Num', collapse_columns=False)

		write_SimCenter_DL_output(
		DV_name+'_agg.csv', DV_mod.T.groupby(level=0).aggregate(np.sum).T,
		index_name='#Num', collapse_columns=False)

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