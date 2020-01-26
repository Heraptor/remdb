#ifndef UTILS_H
#define UTILS_H

#include<iostream>
#include<string>
#include<cstdio>
#include<cstdlib>

#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/container/stable_vector.hpp>

using namespace std;
namespace fs=boost::filesystem;
namespace cntr=boost::container;

const int N=512;
//Launch
void execmd(string incmd){

    char line[N];
    FILE* fp;
    string cmd=incmd;
    const char *syscmd=cmd.data();
    if((fp=popen(syscmd,"r"))==NULL){
        cout<<"error"<<endl;
        return;
    }
    while(fgets(line,sizeof(line)-1,fp)!=NULL){
        cout<<line<<endl;
    }
    pclose(fp);
}
//List files
void recursedir(fs::path src_path,cntr::stable_vector<string>* st_vec){

    fs::directory_iterator end;
    for(fs::directory_iterator dir(src_path);dir!=end;dir++){
        string file_path=dir->path().string();
        string file_ext=dir->path().extension().string();
        int end_dir=file_path.find_last_of("/");
        int end_file=file_path.find_last_of(".");
        string file_name=file_path.substr(end_dir+1);
        //cout<<file_name<<endl;
        if(boost::equals(file_ext,".wt") && boost::starts_with(file_name,"collection") && !boost::contains(file_path,"configsvr"))
            st_vec->push_back(file_path);
        if(fs::is_directory(*dir))
            recursedir(*dir,st_vec);
    }
}

#endif // UTILS_H
