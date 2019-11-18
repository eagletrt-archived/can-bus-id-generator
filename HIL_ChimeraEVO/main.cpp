#include <QCoreApplication>
#include "nlohmann/json.hpp"
#include <QFile>
#include <QIODevice>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <iostream>
#include <QString>
#include <vector>
#include <QDebug>
#include <array>

using json = nlohmann::json;
using namespace std;

//DEVICE'S MASKS
typedef struct mask_t{
    int atc;
    int contr;
    int acu;
    int sw;
    int bms_lv;
    int bms_hv;
    int tel;
}mask_stc;

//DEVICE'S FILTERS
typedef struct filter_t{
    int atc;
    int contr;
    int acu;
    int sw;
    int bms_lv;
    int bms_hv;
    int tel;
}filter_stc;

//MESSAGES STRUCT
typedef struct msg_t{
    QString name;
    int priority;
    QString device;
    int id;
    vector <QString> send_to;
    int to_mod;
    QString byte0;
    QString byte1;
    QString byte2;
    QString byte3;
    QString byte4;
    QString byte5;
    QString byte6;
    QString byte7;
}msg_stc;

//MESSAGES STRUCT
typedef struct use_t{
    int mask0;
    int mask1;
    int mask2;
    int mask3;
    int mask4;
    int mask5;
    int mask6;
    int tot_mask;
    int filt0;
    int filt1;
    int filt2;
    int filt3;
    int filt4;
    int filt5;
    int filt6;
    int id;
}use_stc;

//VECTORS DECLARATIONS
mask_stc mask;
filter_stc filter;
use_stc use;
vector<msg_stc> old_msg;
vector<msg_stc> new_msg;
msg_stc tmp_msg;

int generate_ID(vector<msg_stc> old_msg_ck, int index_ck, QString rec, int flag){

    //OR delle maschere
    if(0 == (rec).compare("ACU")){
        use.mask0 = mask.acu;
    }
    if(0 == (rec).compare("ATC")){
        use.mask1 = mask.atc;
    }
    if(0 == (rec).compare("CONTR")){
        use.mask2 = mask.contr;
    }
    if(0 == (rec).compare("SW")){
        use.mask3 = mask.sw;
    }
    if(0 == (rec).compare("BMS_HV")){
        use.mask4 = mask.bms_hv;
    }
    if(0 == (rec).compare("BMS_LV")){
        use.mask5 = mask.bms_lv;
    }
    if(0 == (rec).compare("TEL")){
        use.mask6 = mask.tel;
    }

    use.tot_mask = use.mask0 | use.mask1 | use.mask2 | use.mask3 | use.mask4 | use.mask5 | use.mask6;
    flag = flag << 5;
    use.id = flag | use.tot_mask;

    return use.id;
}

int main(int argc, char *argv[])
{
    ///Setting device masks (if only IDs are needed, without masks and filters, just take every mask to 0, all msg will pass)///

    mask.tel = 0b00000000000;       //0
    mask.acu = 0b00000000001;       //1
    mask.bms_hv = 0b00000000010;    //2
    mask.contr = 0b00000000100;     //4
    mask.atc = 0b00000001000;       //8
    mask.sw = 0b00000010000;        //16
    mask.bms_lv = 0b00000100000;    //32

    ///Setting device filters///

    filter.tel = 0b00000011111;     //2047
    filter.acu = 0b00000011111;     //2047
    filter.bms_hv = 0b00000011111;  //2047
    filter.contr = 0b00000011111;   //2047
    filter.atc = 0b00000011111;     //2047
    filter.sw = 0b00000011111;      //2047
    filter.bms_lv = 0b00000011111;  //2047

    ///Opening .json files where messages are stored and storing the .json structure///

    QString val;
    QFile file;
    vector<QString> files;

    files.push_back("../import/sensors.json");
    int tmp = 0;
    for (int i= 0; i < files.size(); i++){
        file.setFileName(files[i]);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        val = file.readAll();
        file.close();
        QJsonDocument set = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject data_tmp = set.object();
        QJsonValue value = data_tmp.value("array");
        QJsonArray data = value.toArray();

        foreach(const QJsonValue & v, data)
        {
            msg_stc tmp_msg = {v.toObject().value("name").toString(), v.toObject().value("priority").toInt(),
                               v.toObject().value("device").toString(), v.toObject().value("id").toInt(),
                               vector<QString>(), v.toObject().value("to_mod").toInt(),
                               v.toObject().value("byte0").toString(), v.toObject().value("byte1").toString(),
                               v.toObject().value("byte2").toString(), v.toObject().value("byte3").toString(),
                               v.toObject().value("byte4").toString(), v.toObject().value("byte5").toString(),
                               v.toObject().value("byte6").toString(), v.toObject().value("byte7").toString()};
            foreach(const QJsonValue &send, v.toObject().value("send_to").toArray())
            {
                tmp_msg.send_to.push_back(send.toString());
            }
            old_msg.push_back(tmp_msg);
            tmp++;
        }
    }


    qDebug() << "Messages to compute: " << tmp;
    qDebug() << "";
    qDebug() << "/////////////////INITILAIZING STRUCT-JSON CORRELATIONS////////////////////";
    qDebug() << "";

    int corr[tmp][10];                      //Correletion matrix
    for (int i = 0; i < tmp; i++) {         //Initializing corr matrix with -1 just for prevention
        for (int k = 0; k < 10; k++) {
            corr[i][k] = -1;
        }
    }
    for (uint16_t i = 0; i < tmp; i++){     //Storing correlation datas
        corr[i][0] = old_msg[i].priority;   //Message priority
        corr[i][1] = i;                     //Message index pointing to struct
        corr[i][2] = old_msg[i].to_mod;     //The message will be modified or not
    }

    ///DEBUG SECTION///

    for (int i = 0; i < tmp; i++) {
        qDebug() << "Priority: " << corr[i][0] << "\t | \t Index: " << corr[i][1] << "\t | \t To modify? NO=0/YES=1: " << corr[i][2];
    }

                //////////////////////////////////////////////////////////////////////////////
              ///priority1///priority2///priority3///...
            //////////////////////////////////////////////////////////////////////////////
          ///send_to1 ///send_to2 ///send_to3 ///...
        //////////////////////////////////////////////////////////////////////////////
      /// to_mod  /// to_mod  /// to_mod  ///...
    //////////////////////////////////////////////////////////////////////////////

    ///Sorting corr[][] by max priority from the biggest to the smallest///

    qDebug() << "";
    qDebug() << "///////////////////////////////SORTED-STRUCT//////////////////////////////";
    qDebug() << "";

    int corr_sort[tmp][3];             //Sorted matrix,
    for (int i = 0; i < tmp; i++) {    //Initializing corr matrix with -1 for just for prevention
        for (int k = 0; k < 3; k++) {
            corr_sort[i][k] = -1;
        }
    }
    int max_Prt[1][3];                 //Tmp max priority
    for (int i = 0; i < 3; i++) {
        max_Prt[0][i] = -1;
    }
    int check[tmp];                     //Tmp check
    int check_tot[tmp];                 //Final check
    for (int i = 0; i < tmp; i++) {
        check_tot[i] = 0;               //Initializing check_tot to 0, if 1 the object is not considered in sorting
    }

    for (uint16_t i = 0; i < tmp; i++){
        max_Prt[0][0] = 0;                                          //Initializing max_Prt to 0
        for (uint16_t k = 0; k < tmp; k++){
            if (max_Prt[0][0] <= corr[k][0] && check_tot[k] == 0){  //If the stored max priority is less then the current one and it's an object to check then...
                for (int m = 0; m < 3; m++){
                    max_Prt[0][m] = corr[k][m];                     //Storing correletions and priority in max_Ptr
                }
                check[k] = 1;                                       //Flagging that element was checked
                for (int l = 0; l < k; l++){
                    check[l] = 0;                                   //Cleaning all 1s that are not desired (those are to re-check)
                }
            }
        }
        for (int r = 0; r < tmp; r++) {
            if (check[r] == 1){
                check_tot[r] = 1;               //Storing 1s check_tot for next turn
            }
        }
        for (int m = 0; m < 3; m++) {
            corr_sort[i][m] = max_Prt[0][m];    //Storing elements in the new sorted matrix (corr_sort)
        }
    }

    ///DEBUG SECTION///

    for (int i = 0; i < tmp; i++) {
        qDebug() << "Priority: " << corr_sort[i][0] << "\t | \t Index: " << corr_sort[i][1] << "\t | \t To modify? NO=0/YES=1: " << corr_sort[i][2];
    }

    ///Generating all the IDs for the stored and ordered messages///

    qDebug() << "";
    qDebug() << "//////////////////////////////ID-ASSIGNEMENT//////////////////////////////";
    qDebug() << "";

    int flagID = 0;
    int ID = 0;

    for (int i = 0; i < tmp; i++) {                                     //Cycling all messages
        for (int k = 0; k < old_msg[i].send_to.size(); k++) {           //Cycling all destination for each messages
            if (old_msg[i].to_mod == 1){
                QString rec = old_msg[i].send_to[k];
                ID = generate_ID(old_msg, corr_sort[i][1], rec, flagID);    //Function generating all IDs
            }
            else {
                ID = old_msg[i].id;
            }
        }
        flagID++;

        ///In case of IDs conflicts this part cancel the problem giving a different ID///

        for (int j = 0; j < new_msg.size(); j++) {
            if (ID == new_msg[j].id){
                ID = ID + (1 << 5);                 //It is possible to find a better way to regenerate the id
            }
        }

        ///Resetting id and masks for the next message///

        use.mask0 = 0;
        use.mask1 = 0;
        use.mask2 = 0;
        use.mask3 = 0;
        use.mask4 = 0;
        use.mask5 = 0;
        use.mask6 = 0;
        use.tot_mask = 0;
        use.id = 0;

        ///Storing all new datas of the messages///

        tmp_msg.name = old_msg[i].name;
        tmp_msg.byte0 = old_msg[i].byte0;
        tmp_msg.byte1 = old_msg[i].byte1;
        tmp_msg.byte2 = old_msg[i].byte2;
        tmp_msg.byte3 = old_msg[i].byte3;
        tmp_msg.byte4 = old_msg[i].byte4;
        tmp_msg.byte5 = old_msg[i].byte5;
        tmp_msg.byte6 = old_msg[i].byte6;
        tmp_msg.byte7 = old_msg[i].byte7;
        tmp_msg.priority = corr_sort[i][0];
        tmp_msg.device = old_msg[i].device;
        tmp_msg.send_to = old_msg[i].send_to;
        tmp_msg.to_mod = old_msg[i].to_mod;
        tmp_msg.id = ID;
        new_msg.push_back(tmp_msg);
    }

    int counter = 0;
    for (size_t i = 0; i < tmp; i++) {
        qDebug() << "..........................................................................";
        qDebug() << "";
        qDebug() << "NAME:" << new_msg[i].name;
        qDebug() << "PRIORITY: " << new_msg[i].priority;
        qDebug() << "DEVICE: " << new_msg[i].device;
        qDebug() << "ID: "<< new_msg[i].id;
        qDebug() << "SEND_TO: " << new_msg[i].send_to;
        qDebug() << "TO_MOD:" << new_msg[i].to_mod;
        qDebug() << "B0: " << new_msg[i].byte0;
        qDebug() << "B1: " <<  new_msg[i].byte1;
        qDebug() << "B2: " <<  new_msg[i].byte2;
        qDebug() << "B3: " <<  new_msg[i].byte3;
        qDebug() << "B4: " <<  new_msg[i].byte4;
        qDebug() << "B5: " <<  new_msg[i].byte5;
        qDebug() << "B6: " <<  new_msg[i].byte6;
        qDebug() << "B7: " <<  new_msg[i].byte7;
        counter++;
    }

    qDebug() << "";
    qDebug() << "Messages stored from JSONs: " << tmp;
    qDebug() << "Messages computed: " << counter;
    qDebug() << "Corrupt messages: " << tmp - counter;
    qDebug() << "";
    qDebug() << "Generating header file.......";

    QString bufferBegin = "#ifndef _EAGLE_TRT_CAN_MSG_H\n#define _EAGLE_TRT_CAN_MSG_H\n";
    QString bufferBody = "\n";
    for (int i = 0; i < tmp; i++) {
        bufferBody = bufferBody + "#define " + new_msg[i].name + " " + QVariant(new_msg[i].id).toString() + " \t //" + new_msg[i].device + " sending to: ";
        for (int k = 0; k < new_msg[i].send_to.size(); k++) {
            bufferBody = bufferBody + new_msg[i].send_to[k] + " ";
        }
        bufferBody = bufferBody + "--> B0: " + new_msg[i].byte0 + " | B1: " + new_msg[i].byte1 + " | B2: " + new_msg[i].byte2 + " | B3: " + new_msg[i].byte3 +
                " | B4: " + new_msg[i].byte4 + " | B5: " + new_msg[i].byte5 + " | B6: " + new_msg[i].byte6 + " | B7: " + new_msg[i].byte7 + "\n";
    }
    QString bufferEnd = "\n#endif";


    QString filename = "../E-agleTRTCanMsg.h";
    QFile fileWrite(filename);
    if (fileWrite.open(QIODevice::ReadWrite | QFile::Truncate)) {
        QTextStream stream(&fileWrite);

        stream << bufferBegin + bufferBody + bufferEnd<< endl;
    }
    return 0;
}
