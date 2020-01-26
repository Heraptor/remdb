#ifndef RECOVERINGMDB_H
#define RECOVERINGMDB_H

#include<string>
#include<iostream>
#include<cstdio>
#include<cstdlib>

#include "constants.h"

using namespace  std;
class RecoveringMdb
{
private:
    string dir;
    string file_path;
    string file_without_postfix;
    string restore_collection_file_name;
    string repair_dbf_cmd;
    string dump_dbf_cmd;
    string wb_dbf_cmd;
    void fire(string incmd);
    string repair_ori_dbf();
    string gen_dump_dbf();
    string write_back_dbf();
    string gen_new_collection();
    void launch_prepare();
    void clean_dump();

public:
    int total;
    int progress;
    RecoveringMdb(string file_path);
    RecoveringMdb(string file_path,int total);
    void set_progress(int progress);
    void launch_recovery(int flag);
    void set_restore_collection_file_name(string file_path);
};

#endif // RECOVERINGMDB_H
