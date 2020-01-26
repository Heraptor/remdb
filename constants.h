#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DB_ENGINE   "/root/app/wiredtiger-3.2.1/wt -v -h "
#define DB_DUMP_DIR " /home/ssd/dump/"
#define DB_EXT_SO   " -C \"extensions=[/root/app/wiredtiger-3.2.1/ext/compressors/snappy/.libs/libwiredtiger_snappy.so]\" "
#define DB_REPAIR   " -R salvage "
#define DB_DUMP     " -R dump -f "
#define DB_DST_DIR  " /mnt/mdb/storage/mongo "
#define DB_LOAD     " -R load -f "
#define DB_CHW_DBF  "sudo chown -R mongod:mongod /mnt/mdb/storage/mongo"
#define DB_STO_DBF  "sudo systemctl stop  mongod"
#define DB_STA_DBF  "sudo systemctl start mongod"


#endif // CONSTANTS_H
