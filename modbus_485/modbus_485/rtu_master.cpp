/*
 * @Author: zhou yang 
 * @Date: 2023-08-04 00:30:27 
 * @Last Modified by: zhou yang
 * @Last Modified time: 2023-08-04 01:04:58
 */

#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>

#include "include/modbus/modbus.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "include/sensor.h"
#include "include/switch485.h"

using namespace std;

string readFileIntoString(const string& path);
bool read_config(string file_name_);

char * SERIEL_PORT = "/dev/ttyUSB0";
int BAUD_RATE = 9600;

char * JXBS_3001_TR_PATH = "./tmp/sse1.info";
char * VMS_3000_TR_3W3S_PATH =  "./tmp/sse2.info";
char * WX_WQX7_PATH =  "./tmp/mse1.info";

const int slave_addr = 1;

int LOOP_TIME = 60;
uint32_t RESPONSE_TIME = 1;

JXBS_3001_TR_Sensor * s1;
VMS_3000_TR_3W3S_Sensor * s2;
WX_WQX7_Sensor * s3;

bool read_config(string file_name_) {
    string data;
    data = readFileIntoString(file_name_);

    rapidjson::Document doc;
    if(!doc.Parse(data.data()).HasParseError()) {
        SERIEL_PORT = (char *) malloc(strlen(doc["SERIEL_PORT"].GetString()) + 1);
        strcpy(SERIEL_PORT, doc["SERIEL_PORT"].GetString());
        
        BAUD_RATE = doc["BAUD_RATE"].GetInt();

        JXBS_3001_TR_PATH = (char *) malloc(strlen(doc["JXBS_3001_TR_PATH"].GetString()) + 1);
        strcpy(JXBS_3001_TR_PATH, doc["JXBS_3001_TR_PATH"].GetString());

        VMS_3000_TR_3W3S_PATH = (char *) malloc(strlen(doc["VMS_3000_TR_3W3S_PATH"].GetString()) + 1);
        strcpy(VMS_3000_TR_3W3S_PATH, doc["VMS_3000_TR_3W3S_PATH"].GetString());

        WX_WQX7_PATH = (char *) malloc(strlen(doc["WX_WQX7_PATH"].GetString()) + 1);
        strcpy(WX_WQX7_PATH, doc["WX_WQX7_PATH"].GetString());

        LOOP_TIME = doc["LOOP_TIME"].GetInt();

        RESPONSE_TIME = doc["RESPONSE_TIME"].GetInt();

    } else {
        cout << "Parse error" << endl;
        return false;
    }
    
    return true;
}

bool output_wx_to_file(const WX_WQX7_Sensor *s, const string & path_) {
    rapidjson::StringBuffer str_buff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(str_buff);

    writer.StartObject();

    writer.Key("wind_speed");
    writer.Double(s->wind_speed * 1.0 / 100);

    writer.Key("wind_direction");
    writer.Int64(s->wind_direction);

    writer.Key("temperature");
    writer.Double(s->temperature * 1.0 / 100);

    writer.Key("humidity");
    writer.Double(s->humidity * 1.0 / 100);

    writer.Key("pressure");
    writer.Double(s->pressure * 1.0 / 100);

    writer.Key("rainfall");
    int64_t rainfall_ = (s->rainfall_high << 16 + s->rainfall_low) / 100;
    writer.Int64(rainfall_);

    writer.Key("total_radiation");
    // 1 W/m2 = 683 lux
    double total_radiation_ = 
                ((s->total_radiation_high << 16) + s->total_radiation_low) *1.0 / 683; 
    writer.Double(total_radiation_);

    writer.EndObject();

    int out;
    out = open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);  
    if (-1 == out) 
    {     
        return -1;   
    }
    flock(out, LOCK_EX);
    write(out, str_buff.GetString(), str_buff.GetSize());
    close(out);
    flock(out, LOCK_UN);

    return true;
}

bool output_jxbs_to_file(const JXBS_3001_TR_Sensor *s, const string & path_) {
    rapidjson::StringBuffer str_buff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(str_buff);

    writer.StartObject();

    writer.Key("ph");
    writer.Double(s->pH * 1.0 / 100);

    writer.Key("soil_cond");
    writer.Int64(s->electric_conductivity);

    writer.Key("nitrogen");
    writer.Int64(s->nitrogen);

    writer.Key("phosphorus");
    writer.Int64(s->phosphorus);
    
    writer.Key("potassium");
    writer.Int64(s->potassium);

    writer.EndObject();

    int out;
    out = open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);  
    if (-1 == out)
    {     
        return -1;   
    }
    flock(out, LOCK_EX);
    write(out, str_buff.GetString(), str_buff.GetSize());
    close(out);
    flock(out, LOCK_UN);

    return true;
}

bool output_vms_to_file(const VMS_3000_TR_3W3S_Sensor *s, const string & path_) {
    rapidjson::StringBuffer str_buff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(str_buff);

    writer.StartObject();

    writer.Key("soil_temp1");
    writer.Double(s->temperature_1 * 1.0 / 10);

    writer.Key("soil_humi1");
    writer.Double(s->moisture_1 * 1.0 / 10);

    writer.Key("soil_temp2");
    writer.Double(s->temperature_2 * 1.0 / 10);

    writer.Key("soil_humi2");
    writer.Double(s->moisture_2 * 1.0 / 10);

    writer.Key("soil_temp3");
    writer.Double(s->temperature_3 * 1.0 / 10);

    writer.Key("soil_humi3");
    writer.Double(s->moisture_3 * 1.0 / 10);

    writer.EndObject();

    int out;
    out = open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);  
    if (-1 == out)   
    {     
        return -1;   
    }
    flock(out, LOCK_EX);
    write(out, str_buff.GetString(), str_buff.GetSize());
    close(out);
    flock(out, LOCK_UN);

    return true;
}

string readFileIntoString(const string& path) {
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int main(int argc, char *argv[]) {
    modbus_t *ctx;
    int rc;
    uint16_t tab_reg[512];
    memset(tab_reg, 0, 1024);

    s1 = (JXBS_3001_TR_Sensor *) malloc(sizeof(JXBS_3001_TR_Sensor));
    s2 = (VMS_3000_TR_3W3S_Sensor *) malloc(sizeof(VMS_3000_TR_3W3S_Sensor));
    s3 = (WX_WQX7_Sensor *) malloc(sizeof(WX_WQX7_Sensor));

    read_config("./config.json");

    // Open
    ctx = modbus_new_rtu(SERIEL_PORT, BAUD_RATE, 'N', 8, 1);
    
    // Set 485 mode
    _server_ioctl_init();
    modbus_rtu_set_custom_rts(ctx, _modbus_rtu_server_ioctl);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
    modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    
    modbus_set_response_timeout(ctx, RESPONSE_TIME, 0);
    
    // Debug
    modbus_set_debug(ctx, 1);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        exit(-1);
    }
    printf("Open seriel port success ~\n");

    while (TRUE) {  
    // Loop for each 60s
#ifdef JXBS_3001_TR
    memset(s1, 0, sizeof(JXBS_3001_TR_Sensor));
    if (JXBS_get(ctx, s1) == FALSE) {
        printf("\e[1;31mFail\e[0m to get JXBS data\n\n");
    } else {
        JXBS_print(s1);
        output_jxbs_to_file(s1, JXBS_3001_TR_PATH);
    }
#endif

#ifdef VMS_3000_TR_3W3S    
    memset(s2, 0, sizeof(VMS_3000_TR_3W3S_Sensor));
    if (VMS_get(ctx, s2) == FALSE) {
        printf("\e[1;31mFail\e[0m to get VMS data\n\n");
    } else {
        VMS_print(s2);
        output_vms_to_file(s2, VMS_3000_TR_3W3S_PATH);
    }
#endif

#ifdef WX_WQX7    
    memset(s3, 0, sizeof(WX_WQX7_Sensor));
    if (WX_get(ctx, s3) == FALSE) {
        printf("\e[1;31mFail\e[0m to get WX data\n\n");
    } else {        
        output_wx_to_file(s3, WX_WQX7_PATH);
        WX_print(s3);
    }
#endif

    printf("Sleep %d seconds ~\n", LOOP_TIME);
    for (uint i = 0; i < LOOP_TIME; ++i) {
        printf("#"); fflush(stdout);
        sleep(1);
    }
    printf("\n\n"); 
    } // End of while (TRUE)

    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
