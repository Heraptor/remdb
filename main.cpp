#include<iostream>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<boost/shared_ptr.hpp>
#include<boost/lexical_cast.hpp>

#include<bson/bson.h>
#include<mongoc/mongoc.h>
#include<stdio.h>

#include<bsoncxx/builder/stream/document.hpp>
#include<bsoncxx/json.hpp>
#include<mongocxx/client.hpp>
#include<mongocxx/instance.hpp>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/json_parser.hpp>
#include<ctime>

#include "utils.h"
#include "recoveringmdb.h"

#define MONODB_URL  "mongodb://qther:Trimps076ReYespayMdb4qth@35.202.0.102:27017/?authSource=admin"
#define MONODB_NAME "yinshengepay"
#define MONODB_COLLECT_INDEX "recovery_monodb_collection_index"
#define MONODB_ENGINE_URI "wiredTiger.uri"
#define MONODB_RECOVERY_COLLS "yespay_colls_"

namespace bbs=bsoncxx::builder::stream;
namespace pt=boost::property_tree;

static mongocxx::instance inst{};

/*
 *
 * 1 use ssd  to work.
 *   >/home/ssd
 * 2 use disk to store bak.
 *   >store:101:/home/mdb/
 *   >bask:101: /mnt/mdb
 * 3 use disk to store db.
 *   >/mnt/mdb/storage/mongo
 *!4 collection data structure may be different.
 * 5 dump dir
 *   >/home/ssd/dump
 *
 * [root@mdb1 8oridisk.bak]# du -h --max-depth=1
 * 1.8T    ./2T-01
 * 1.8T    ./2T-02
 * 1.5T    ./2T-03
 * 1.7T    ./2T-04
 * 574G    ./2T-05
 * 3.0T    ./4T-01
 * 3.2T    ./4T-02
 * 3.3T    ./4T-03
 * 17T     .
 * woking--------------------------------------------------------->
 * 1 102, 2T-01
 */
string read_json_with_key(string json,string key){
    pt::ptree root;
    std::stringstream ss;
    ss<<json;
    pt::read_json(ss,root);
    std::string msg=root.get<std::string>(key);
    return msg;
}

void write_collection(string collect){
    string uri=MONODB_URL;
    mongocxx::client conn{mongocxx::uri{uri}};
    mongocxx::database monodb =conn[MONODB_NAME];
    mongocxx::collection collection=monodb[collect];

    bbs::document document{};
    //auto collection=conn["yinshengepay"]["suspect"];
    document<<"Fresh"<<"Init!";
    collection.insert_one(document.view());
    auto cursor=collection.find({});
    for(auto&& doc : cursor){
        cout<<bsoncxx::to_json(doc)<<endl;
    }
    bbs::document doc{} ;
    collection.delete_one(doc<<"hello"<<"world"<<bbs::finalize);
}

string read_collection_index_4recovery(string collect){
    string uri=MONODB_URL;
    mongocxx::client conn{mongocxx::uri{uri}};
    mongocxx::database monodb =conn[MONODB_NAME];
    mongocxx::collection collection=monodb[collect];
    bbs::document document{};
    auto cursor=collection.find({});
    for(auto&& doc : cursor){
        cout<<bsoncxx::to_json(doc)<<endl;
    }
    bsoncxx::stdx::optional<bsoncxx::document::value> rel=collection.find_one({});
    string json;
    if(rel){
        json = bsoncxx::to_json(*rel);
        cout<<"read collection : "<<bsoncxx::to_json(*rel)<<endl;
    }
    return json;
}


string inspect_db_command(string collect){
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *command;
    bson_t reply;
    char* info;

    mongoc_init();

    client = mongoc_client_new (MONODB_URL);
    collection = mongoc_client_get_collection (client, MONODB_NAME, (char*)collect.c_str());

    command = BCON_NEW ("collStats", BCON_UTF8 ((char*)collect.c_str()));
    if (mongoc_collection_command_simple (collection, command, NULL, &reply, &error)) {
        info = bson_as_canonical_extended_json (&reply, NULL);
        //printf ("%s\n", info);

    } else {
        fprintf (stderr, "Failed to run command: %s\n", error.message);
    }

    //    bson_t *bt;
    //    bson_iter_t iter;
    //    int slen=strlen(info)+1;
    //    uint8_t *msg=new uint8_t[slen];
    //    memcpy(msg,info,slen);
    //    printf ("%s\n", msg);
    //    if ((bt = bson_new_from_data (msg, slen))) {
    //        if (bson_iter_init (&iter, bt)) {
    //            while (bson_iter_next (&iter)) {
    //                printf
    //                printf ("Found element key: \"%s\"\n", bson_iter_key (&iter));
    //            }
    //        }
    //        bson_destroy (bt);
    //    }

    string msg=boost::lexical_cast<string>(info);
    bson_free (info);
    bson_destroy (command);
    bson_destroy (&reply);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
    mongoc_cleanup ();
    return msg;
}

void refresh_collection(string collect){
    string uri=MONODB_URL;
    mongocxx::client conn{mongocxx::uri{uri}};
    mongocxx::database monodb =conn[MONODB_NAME];
    mongocxx::collection collection=monodb[collect];

    bbs::document document{};
    //auto collection=conn["yinshengepay"]["suspect"];
    document<<"Fresh"<<"Init!";
    collection.insert_one(document.view());
    auto cursor=collection.find({});
    for(auto&& doc : cursor){
        cout<<bsoncxx::to_json(doc)<<endl;
    }
    bbs::document doc{} ;
    collection.delete_one(doc<<"Fresh"<<"Init!"<<bbs::finalize);

    //mongocxx::cursor cursor_pref=collection.read_preference().tags().value();
    /*
    collection.read_concern();
    mongocxx::cursor cursor_doc=collection.list_indexes();
    for(auto doc:cursor_doc){
        cout<<bsoncxx::to_json(doc)<<endl;
    }
    mongocxx::cursor cursor_coll=monodb.list_collections();
    for(auto doc:cursor_coll){
        cout<<bsoncxx::to_json(doc)<<endl;
    }
    */
}

string fetch_collection_file_name(string collecttion_name){
    //init collection with this collecttion_name
    refresh_collection(collecttion_name);
    string collect_stats=inspect_db_command(collecttion_name);
    //read exist collection name
    string collection_file_name=read_json_with_key(collect_stats,MONODB_ENGINE_URI);
    int end_tag=collection_file_name.find_last_of(":")+1;
    collection_file_name=collection_file_name.substr(end_tag);
    return collection_file_name;
}

vector<string> list_db_collections(){
    string uri=MONODB_URL;
    mongocxx::client conn{mongocxx::uri{uri}};
    mongocxx::database monodb =conn[MONODB_NAME];
    return monodb.list_collection_names();
}

int fetch_collection_index_4recovery(){
    string index=read_collection_index_4recovery(MONODB_COLLECT_INDEX);
    string collection_index=read_json_with_key(index,"collection_index");
    return boost::lexical_cast<int>(collection_index);
}

void increase_collection_index_4recovery(int index){
    string uri=MONODB_URL;
    mongocxx::client conn{mongocxx::uri{uri}};
    mongocxx::database monodb =conn[MONODB_NAME];
    mongocxx::collection collection=monodb[MONODB_COLLECT_INDEX];
    bbs::document document0{};
    document0<<"collection_index"<<boost::lexical_cast<string>(index-1)<<bbs::finalize;
    bbs::document document1{};
    document1<<"$set"<<bbs::open_document<<"collection_index"<<boost::lexical_cast<string>(index)<<bbs::close_document<<bbs::finalize;
    //collection.update_one(document0,document1);
    collection.update_one(bbs::document {} <<"collection_index"<<boost::lexical_cast<string>(index-1)<<bbs::finalize,
                          bbs::document {} <<"$set"<<bbs::open_document<<"collection_index"<<boost::lexical_cast<string>(index)<<bbs::close_document<<bbs::finalize);
}
void recovery_mongodb(string path){
    cntr::stable_vector<string> st_vec;
    cntr::stable_vector<boost::shared_ptr<RecoveringMdb>> st_vec_revocerdb;
    fs::path src_path=path;
    //db files
    recursedir(src_path,&st_vec);
    cout<<"---"<<st_vec.size()<<endl;
    //init && prepare
    for(auto it_auto:st_vec){
        string it_str=static_cast<string>(it_auto);
        boost::shared_ptr<RecoveringMdb> revocerdb(new RecoveringMdb(it_str,st_vec.size()));
        st_vec_revocerdb.push_back(revocerdb);
    }
    //repair,dump,wb
    int progress=1;
    for(auto it_auto:st_vec_revocerdb){
        boost::shared_ptr<RecoveringMdb> it_recover_db=static_cast<boost::shared_ptr<RecoveringMdb>>(it_auto);
        it_recover_db->set_progress(progress++);
        int current_index=fetch_collection_index_4recovery();
        string coll=MONODB_RECOVERY_COLLS;
        string cool_name=coll.append(boost::lexical_cast<string>(current_index++));
        string restore_collection_file_name=fetch_collection_file_name(cool_name);
        it_recover_db->set_restore_collection_file_name(restore_collection_file_name);
        increase_collection_index_4recovery(current_index++);
        //write back collection file
        it_recover_db->launch_recovery(1);
        time_t now=time(0);
        char* dt=ctime(&now);
        cout<<"Time: "<<dt<<".Progress: >>>>>>>>>>>>>>> "<<it_recover_db->progress<<"/"<<it_recover_db->total<<",collection is:"<<cool_name<<endl;
        //break;
    }
}


int main(int argc,char* argv[]){

    if(argc>1){
        string src_dir=boost::lexical_cast<string>(argv[1]);
        boost::filesystem::path path(src_dir);
        if(boost::filesystem::exists(path))
        {
            cout << "Start to recovery mdb :) " << endl;
            recovery_mongodb(src_dir);

            //fetch collect_file_name
            //cout<<"collect_file_name str is :> "<<fetch_collection_file_name("yespay_colls_100")<<endl;
            //read_collection_index_4recovery
            //cout<<"collection_index is :> "<<fetch_collection_index_4recovery()<<endl;
            //increase_collection_index_4recovery(101);

            cout << "Success of recovery ! " << endl;
        }else{
            cout << "Please specify the right input directory! " << endl;
        }
    }else{
        cout << "Please specify the right input parameter! " << endl;
    }
    return 0;
}
