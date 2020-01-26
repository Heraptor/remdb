#include "recoveringmdb.h"

RecoveringMdb::RecoveringMdb(string file_path)
{
    int end_dir=file_path.find_last_of("/");
    int end_file=file_path.find_last_of(".");
    string dir=file_path.substr(0,end_dir);
    string file_without_postfix=file_path.substr(end_dir+1,end_file-end_dir-1);
    string file_name=file_path.substr(end_dir+1);
    //std::cout<<" 1 is: "<<file_without_postfix<<std::endl;
    //std::cout<<" 2 is: "<<file_name<<std::endl;
    this->dir=dir;
    this->file_path=file_name;
    this->file_without_postfix=file_without_postfix;
}
RecoveringMdb::RecoveringMdb(string file_path,int total)
{
    int end_dir=file_path.find_last_of("/");
    int end_file=file_path.find_last_of(".");
    string dir=file_path.substr(0,end_dir);
    string file_without_postfix=file_path.substr(end_dir+1,end_file-end_dir-1);
    string file_name=file_path.substr(end_dir+1);
    //std::cout<<" 1 is: "<<file_without_postfix<<std::endl;
    //std::cout<<" 2 is: "<<file_name<<std::endl;
    this->dir=dir;
    this->file_path=file_name;
    this->file_without_postfix=file_without_postfix;
    this->total=total;
}

string RecoveringMdb::repair_ori_dbf( ){
    string recover_cmd="";
    recover_cmd.append(DB_ENGINE);
    recover_cmd.append(dir);
    recover_cmd.append(DB_EXT_SO);
    recover_cmd.append(DB_REPAIR);
    recover_cmd.append(file_path);
    this->repair_dbf_cmd=recover_cmd;    
    return recover_cmd;
}

string RecoveringMdb::gen_dump_dbf( ){
    string recover_cmd="";
    recover_cmd.append(DB_ENGINE);
    recover_cmd.append(dir);
    recover_cmd.append(DB_EXT_SO);
    recover_cmd.append(DB_DUMP);
    recover_cmd.append(DB_DUMP_DIR);
    recover_cmd.append(file_without_postfix);
    recover_cmd.append(".dump");
    recover_cmd.append(" ");
    recover_cmd.append(file_without_postfix);
    this-> dump_dbf_cmd=recover_cmd;    
    return recover_cmd;
}

string RecoveringMdb::write_back_dbf( ){
    string recover_cmd="";
    recover_cmd.append(DB_ENGINE);
    recover_cmd.append(DB_DST_DIR);
    recover_cmd.append(DB_EXT_SO);
    recover_cmd.append(DB_LOAD);
    recover_cmd.append(DB_DUMP_DIR);
    recover_cmd.append(file_without_postfix);
    recover_cmd.append(".dump");
    recover_cmd.append(" -r ");
    recover_cmd.append(restore_collection_file_name);
    this-> wb_dbf_cmd=recover_cmd;
    return recover_cmd;
}

void RecoveringMdb::launch_prepare(){
    repair_ori_dbf( );
    gen_dump_dbf( );
    write_back_dbf( );
}

void RecoveringMdb::set_restore_collection_file_name(string file_name){
    this->restore_collection_file_name=file_name;
}

void RecoveringMdb::clean_dump( ){
    string cmd="rm -f ";
    cmd.append(DB_DUMP_DIR).append("*");
    fire(cmd);
}

void RecoveringMdb::set_progress(int progress){
    this->progress=progress;
}

void RecoveringMdb::launch_recovery(int flag){
    launch_prepare();
    clean_dump();
    if(flag){
        fire(repair_dbf_cmd);
        fire(dump_dbf_cmd);
        fire(DB_STO_DBF);
        fire(wb_dbf_cmd);
        fire(DB_CHW_DBF);
        fire(DB_STA_DBF);
    }
    cout << "Recover one dbf : "<< this->dir<<"/"<<this->file_path<< endl;
}

void RecoveringMdb::fire(string incmd){

    char line[512];
    FILE* fp;
    string cmd=incmd;
    const char *syscmd=cmd.data();
    if((fp=popen(syscmd,"r"))==NULL){
        cout<<"error"<<endl;
        return;
    }
    while(fgets(line,sizeof(line)-1,fp)!=NULL){
        cout<<progress<<"/"<<total<<", cmd run result is : " <<line<<endl;
    }
    pclose(fp);
}
