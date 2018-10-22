import sys
import numpy as np

import pelicunPBE
from pelicunPBE.control import FEMA_P58_Assessment
from pelicunPBE.file_io import write_SimCenter_DL_output


def run_pelicun(DL_input_path, EDP_input_path, 
	CMP_data_path=None, POP_data_path=None):

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

	#write_SimCenter_DL_output('DL_DMG.csv', A._DMG, 
	#	index_name='#Num', collapse_columns=True)

	write_SimCenter_DL_output('DL_summary_stats.csv', A._SUMMARY, 
		index_name='attribute',
		collapse_columns=True, stats_only=True)

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