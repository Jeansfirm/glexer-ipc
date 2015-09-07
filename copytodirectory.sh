#!/bin/bash

cp addIPC_znd_arm_20150619_lastest/addIPC.cgi  ipc_cgi_bin_arm/cgi
cp deleteIPC_znd_arm_lastest/deleteIPC.cgi  ipc_cgi_bin_arm/cgi
cp editIPC_znd_arm_20150623_lastest/editIPC.cgi  ipc_cgi_bin_arm/cgi
cp getIPClist_znd_arm_20150619_lastest/getIPClist.cgi  ipc_cgi_bin_arm/cgi
cp IPCCapture_pjf_arm_latest/IPCCapture.cgi  ipc_cgi_bin_arm/cgi	
cp modifyIPCindex_znd_arm_lastest/modifyIPCindex.cgi  ipc_cgi_bin_arm/cgi
cp online_detect_newfun_znd_arm_lastest/ipc_online  ipc_cgi_bin_arm/bin
cp IPCRecord_pjf_20150814/IPCRecord.cgi  ipc_cgi_bin_arm/cgi

echo "script executed successfully!"
