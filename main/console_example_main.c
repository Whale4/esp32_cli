/* Basic console example (esp_console_repl API)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>     
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "cmd_system.h"
#include "cmd_wifi.h"
#include "cmd_nvs.h"
#include "argtable3/argtable3.h"

static const char* TAG = "example";
#define PROMPT_STR ""
int parameters_number;

int CLI_get_value(char *value_string, int datatype, float* f_val, int * int_val);
void CLI_service_init();

//TEST RESET COMMANDS
void hard_reset();
void soft_reset();

//TEST INTEGER VALUE
int test_RADIO_MUTE_DELAY = 25;
int get_RADIO_MUTE_DELAY();
int set_RADIO_MUTE_DELAY(int val);

//TEST STRING VALUE
char test_ECALL_TEST_NUMBER[20] = {0};
char* get_ECALL_TEST_NUMBER();
void set_ECALL_TEST_NUMBER(char *str_val);

//TEST FLOAT VALUE
float test_ASI15_TRESHOLD = 7.7f;
float get_ASI15_TRESHOLD();
float set_ASI15_TRESHOLD(float val);

/*
static const char *const PIN_enum_names[] = {             
    "NONE",
    "PIN 1",
    "PIN 2",
    "PIN 3",
    "PIN 4",
    "PIN 5",
    "PIN 6",
    "PIN 7",
    "PIN 8"
};

static const char *const STATES_enum_names[] = {             
    "ENABLED",
    "DISABLED"
};

static const char *const STATUS_enum_names[] = {             
    "ACTIVE",
    "INACTIVE"
};

static const char *const GNSS_enum_names[] = {             
    "NO_FIX",
    "FIX_2D",
    "FIX_3D"
};

static const char *const GSM_enum_names[] = {             
    "NO_REG_NO_SEARCH",
    "REG_HOME",
    "SEARCH",
    "DENIED",
    "REG_ROAMING"
};

static const char *const FOTA_enum_names[] = {             
    "IDLE",
    "DOWLOADING",
    "WAITING_IGN_OFF",
    "FLASHING",
    "UPDATE_OK",
    "UPDATE_ERROR",
    "DOWNLOAD_CANCELLED"
};

static const char *const MOUNT_enum_names[] = {             
    "NATIVE",
    "ADDITIONAL"
};

static const char *const CLI_cmd_service_names[] = {             
    "READ",
    "RESET",
    "WRITE",
    "CONTROL",
    "ROUTINE"
};

char *const CLI_resp_status_names[] = {
    "NOK",
    "OK"
}; 

static const char *const CLI_cmd_service_opt2_names[] = {
    "NO_RESP",                
    "RESP",
    "SET",
    "RETURN"
};

*/
static const char *const BOOL_names[] = {             
    "FALSE",
    "TRUE"
};

static const char *const CLI_cmd_param_names[] = {
	"RADIO_MUTE_DELAY",
    "RADIO_UNMUTE_DELAY",
    "CALL_AUTO_ANSWER_TIME",
    "POST_TEST_REGISTRATION_TIME",
    "TEST_MODE_END_DISTANCE",
    "GARAGE_MODE_END_DISTANCE",
    "ECALL_TEST_NUMBER",
    "GARAGE_MODE_PIN",
    "INT_MEM_TRANSMIT_INTERVAL",
    "INT_MEM_TRANSMIT_ATTEMPTS",
    "CRASH_SIGNAL_INTERNAL",
    "CRASH_SIGNAL_EXTERNAL",
    "ASI15_TRESHOLD",
    "ECALL_MODE_PIN",
    "SOS_BUTTON_TIME",
    "CCFT",
    "MSD_MAX_TRANSMISSION_TIME",
    "NAD_DEREGISTRATION_TIME",
    "ECALL_NO_AUTOMATIC_TRIGGERING",
    "ECALL_DIAL_DURATION",
    "ECALL_AUTO_DIAL_ATTEMPTS",
    "ECALL_MANUAL_DIAL_ATTEMPTS",
    "ECALL_MANUAL_CAN_CANCEL",
    "ECALL_SMS_FALLBACK_NUMBER",
    "TEST_REGISTRATION_PERIOD",
    "IGNITION_OFF_FOLLOW_UP_TIME1",
    "IGNITION_OFF_FOLLOW_UP_TIME2",
    "CRASH_RECORD_TIME",
    "CRASH_RECORD_RESOLUTION",
    "CRASH_PRE_RECORD_TIME",
    "CRASH_PRE_RECORD_RESOLUTION",
    "GNSS_POWER_OFF_TIME",
    "GNSS_DATA_RATE",
    "GNSS_MIN_ELEVATION",
    "VIN",
    "VEHICLE_TYPE",
    "VEHICLE_PROPULSION_STORAGE_TYPE",
    "ICCID_ECALL",
    "IMEI_ECALL",
    "ESN",
    "GNSS_FIX",
    "GSM_REG_STATUS",
    "SOS_BUTTON_INPUT_STATUS",
    "FUNC_BUTTON_INPUT_STATUS",
    "IGN_INPUT_STATUS",
    "ECALL_MODE_PIN_OUPUT_STATUS",
    "GARAGE_MODE_PIN_OUTPUT_STATUS",
    "SOS_INDICATOR_OUTPUT_STATUS",
    "ECALL_EMERGENCY_DEBUG",
    "ECALL_EMERGENCY_DEBUG_NUMBER",
    "DEBUG_OUTPUT_ECALL",
    "ECALL_ON",
    "ECALL_SPEAKER_VOLUME",
    "ECALL_MICROPHONE_LEVEL",
    "FOTA_STATUS",
    "CRASH_TURNOVER_THRESHOLD",
    "CRASH_TURNOVER_DURATION",
    "MOUNT_TYPE",
    "CRASH_INPUT_PIN",
    "NUMBER_TROUBLE_CODE",
    "SIGNAL_STRENGTH",
    "SUPPLY_VOLTAGE",
    "BOOT_VERSION",
    "CLAIBRATION_NUMBER",
    "MODULE_NUMBER",
    "STRATEGY_NUMBER",
    "HARDWARE_NUMBER",
    "MODEM_SOFTWARE_NUMBER",
    "IMU_SOFTWARE_NUMBER",
    "MSD_NUMBER",
    "LAST_MSD",
    "CURRENT_GNSS_LAT",
    "CURRENT_GNSS_LONG",
    "CURRENT_GNSS_TIME"
};

typedef enum {
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_FLOAT,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_BOOL
} CLI_param_type;

int CLI_param_value_type[] = {
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_BOOL,
    CLI_PARAM_TYPE_BOOL,
    CLI_PARAM_TYPE_FLOAT,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_BOOL,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_BOOL,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_ENUM,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_INT,
    CLI_PARAM_TYPE_STRING,
    CLI_PARAM_TYPE_FLOAT,
    CLI_PARAM_TYPE_FLOAT,
    CLI_PARAM_TYPE_INT
};



typedef enum {
    CLI_PARAM_RADIO_MUTE_DELAY,
    CLI_PARAM_RADIO_UNMUTE_DELAY,
    CLI_PARAM_CALL_AUTO_ANSWER_TIME,
    CLI_PARAM_POST_TEST_REGISTRATION_TIME,
    CLI_PARAM_TEST_MODE_END_DISTANCE,
    CLI_PARAM_GARAGE_MODE_END_DISTANCE,
    CLI_PARAM_ECALL_TEST_NUMBER,
    CLI_PARAM_GARAGE_MODE_PIN,
    CLI_PARAM_INT_MEM_TRANSMIT_INTERVAL,
    CLI_PARAM_INT_MEM_TRANSMIT_ATTEMPTS,
    CLI_PARAM_CRASH_SIGNAL_INTERNAL,
    CLI_PARAM_CRASH_SIGNAL_EXTERNAL,
    CLI_PARAM_ASI15_TRESHOLD,
    CLI_PARAM_ECALL_MODE_PIN,
    CLI_PARAM_SOS_BUTTON_TIME,
    CLI_PARAM_CCFT,
    CLI_PARAM_MSD_MAX_TRANSMISSION_TIME,
    CLI_PARAM_NAD_DEREGISTRATION_TIME,
    CLI_PARAM_ECALL_NO_AUTOMATIC_TRIGGERING,
    CLI_PARAM_ECALL_DIAL_DURATION,
    CLI_PARAM_ECALL_AUTO_DIAL_ATTEMPTS,
    CLI_PARAM_ECALL_MANUAL_DIAL_ATTEMPTS,
    CLI_PARAM_ECALL_MANUAL_CAN_CANCEL,
    CLI_PARAM_ECALL_SMS_FALLBACK_NUMBER,
    CLI_PARAM_TEST_REGISTRATION_PERIOD,
    CLI_PARAM_IGNITION_OFF_FOLLOW_UP_TIME1,
    CLI_PARAM_IGNITION_OFF_FOLLOW_UP_TIME2,
    CLI_PARAM_CRASH_RECORD_TIME,
    CLI_PARAM_CRASH_RECORD_RESOLUTION,
    CLI_PARAM_CRASH_PRE_RECORD_TIME,
    CLI_PARAM_CRASH_PRE_RECORD_RESOLUTION,
    CLI_PARAM_GNSS_POWER_OFF_TIME,
    CLI_PARAM_GNSS_DATA_RATE,
    CLI_PARAM_GNSS_MIN_ELEVATION,
    CLI_PARAM_VIN,
    CLI_PARAM_VEHICLE_TYPE,
    CLI_PARAM_VEHICLE_PROPULSION_STORAGE_TYPE,
    CLI_PARAM_ICCID_ECALL,
    CLI_PARAM_IMEI_ECALL,
    CLI_PARAM_ESN,
    CLI_PARAM_GNSS_FIX,
    CLI_PARAM_GSM_REG_STATUS,
    CLI_PARAM_SOS_BUTTON_INPUT_STATUS,
    CLI_PARAM_FUNC_BUTTON_INPUT_STATUS,
    CLI_PARAM_IGN_INPUT_STATUS,
    CLI_PARAM_ECALL_MODE_PIN_OUPUT_STATUS,
    CLI_PARAM_GARAGE_MODE_PIN_OUTPUT_STATUS,
    CLI_PARAM_SOS_INDICATOR_OUTPUT_STATUS,
    CLI_PARAM_ECALL_EMERGENCY_DEBUG,
    CLI_PARAM_ECALL_EMERGENCY_DEBUG_NUMBER,
    CLI_PARAM_DEBUG_OUTPUT_ECALL,
    CLI_PARAM_ECALL_ON,
    CLI_PARAM_ECALL_SPEAKER_VOLUME,
    CLI_PARAM_ECALL_MICROPHONE_LEVEL,
    CLI_PARAM_FOTA_STATUS,
    CLI_PARAM_CRASH_TURNOVER_THRESHOLD,
    CLI_PARAM_CRASH_TURNOVER_DURATION,
    CLI_PARAM_MOUNT_TYPE,
    CLI_PARAM_CRASH_INPUT_PIN,
    CLI_PARAM_NUMBER_TROUBLE_CODE,
    CLI_PARAM_SIGNAL_STRENGTH,
    CLI_PARAM_SUPPLY_VOLTAGE,
    CLI_PARAM_BOOT_VERSION,
    CLI_PARAM_CLAIBRATION_NUMBER,
    CLI_PARAM_MODULE_NUMBER,
    CLI_PARAM_STRATEGY_NUMBER,
    CLI_PARAM_HARDWARE_NUMBER,
    CLI_PARAM_MODEM_SOFTWARE_NUMBER,
    CLI_PARAM_IMU_SOFTWARE_NUMBER,
    CLI_PARAM_MSD_NUMBER,
    CLI_PARAM_LAST_MSD,
    CLI_PARAM_CURRENT_GNSS_LAT,
    CLI_PARAM_CURRENT_GNSS_LONG,
    CLI_PARAM_CURRENT_GNSS_TIME,
    CLI_PARAM_NULL,
} CLI_cmd_param;

static const char *const CLI_cmd_service_opt1_names[] = {
    "HARD",                
    "SOFT",
    "START",
    "RESULT"
}; 



static const char *const CLI_resp_err_names[] = {
    "UNKNOWN_ERROR",
    "COMMAND_INCORRECT",
    "SERVICE_INCORRECT",
    "VALUE_OUT_OF_RANGE",
    "WRITE_NOT_PERMIT",
    "COMMON_ERROR",
    "INCORRECT_VALUE_TYPE",
    "RESET_CMD_INCORRECT",
    "TIMEOUT",
    "PARAMETER_NOT_EXIST",
    "VALUE_FORMAT_NOT_VALID"
}; 

typedef enum {
    CLI_RESP_ERR_UNKNOWN_ERROR,
    CLI_RESP_ERR_COMMAND_INCORRECT,
    CLI_RESP_ERR_SERVICE_INCORRECT,
    CLI_RESP_ERR_VALUE_OUT_OF_RANGE,
    CLI_RESP_ERR_WRITE_NOT_PERMIT,
    CLI_RESP_ERR_COMMON_ERROR,
    CLI_RESP_ERR_INCORRECT_VALUE_TYPE,
    CLI_RESP_ERR_RESET_CMD_INCORRECT,
    CLI_RESP_ERR_TIMEOUT,
    CLI_RESP_ERR_PARAMETER_NOT_EXIST,
    CLI_RESP_ERR_VALUE_FORMAT_NOT_VALID,
} CLI_resp_err;

static struct {
    struct arg_str *opt1;
    struct arg_str *opt2;
    struct arg_end *end;
} reset_args;

static struct {
    struct arg_str *parameter;
    struct arg_end *end;
} read_args;

static struct {
    struct arg_str *parameter;
    struct arg_str *value;
    struct arg_end *end;
} write_args;


void get_parameters_count(){
    parameters_number = sizeof(CLI_cmd_param_names)/sizeof(CLI_cmd_param_names[0]);
}

static int reset_set(int argc, char **argv) {
    bool resp = true; //RESPONSE COMMAND (ACTIVE BY DEFAULT)
    int j=0;

    CLI_resp_err err_name = CLI_RESP_ERR_UNKNOWN_ERROR;

    int nerrors = arg_parse(argc, argv, (void **) &reset_args);
    if (nerrors != 0) {
        err_name = CLI_RESP_ERR_COMMAND_INCORRECT;
        goto NOK_response;
    }

    //OPTIONAL ARGUMENT  (RESP or NO_RESP)
    if (reset_args.opt2->count) {
        char opt2[7];
        const char *src = reset_args.opt2->sval[0];
        size_t len = strlen(src);
        if (len > sizeof(opt2)) {
            len = sizeof(opt2);
        }
        memcpy(opt2, src, len);

        if (strstr(opt2, "NO_RESP") != NULL) {
            resp = false;
        }
    }
    
    //MANDATORY ARGUMENT (HARD or SOFT)
    if (reset_args.opt1->count) {
        char opt1[4];
        const char *src = reset_args.opt1->sval[0];
        size_t len = strlen(src);
        if (len > sizeof(opt1)) {
            len = sizeof(opt1);
        }
        memcpy(opt1, src, len);

        for (j=0; j<2; j++){  //j==0 [HARD RESET], j==1 [SOFT RESET]
            if (strstr(opt1, CLI_cmd_service_opt1_names[j]) != NULL) {
                goto OK_response;
                break;
            }
        }
        err_name = CLI_RESP_ERR_PARAMETER_NOT_EXIST;
        goto NOK_response;
    }

NOK_response:
    printf("RESET NOK %s\r\n",CLI_resp_err_names[err_name]);
    return 0;

OK_response:
    if(resp)
        printf("RESET OK %s\r\n",CLI_cmd_service_opt1_names[j]);
    if(j==0){
        hard_reset();
    } else if (j==1) {
        soft_reset();
    }
    return 0;
}

int read_set(int argc, char **argv) {
    int i=0;
    int var_type = -1;

    int tmp_i = 0;
    float tmp_f = 0;
    char *tmp_str = NULL;

    CLI_resp_err err_name = CLI_RESP_ERR_UNKNOWN_ERROR;

    int nerrors = arg_parse(argc, argv, (void **) &read_args);
    if (nerrors != 0) {
        err_name = CLI_RESP_ERR_COMMAND_INCORRECT;
        goto NOK_response;
    }

    //GET PARAMETER 
    if (read_args.parameter->count) {
        char param_name[32] = {0};
        const char *src = read_args.parameter->sval[0];
        size_t len = strlen(src);
        if (len > sizeof(param_name)) {
            len = sizeof(param_name);
        }
        memcpy(param_name, src, len);

        for (i=0; i<parameters_number; i++){  //j==0 [HARD RESET], j==1 [SOFT RESET]
            if (strstr(param_name, CLI_cmd_param_names[i]) != NULL) {
                var_type = CLI_param_value_type[i];
                switch(i){
                    case(CLI_PARAM_RADIO_MUTE_DELAY):
                        tmp_i = get_RADIO_MUTE_DELAY();
                        break;
                    case(CLI_PARAM_RADIO_UNMUTE_DELAY):
                        break;
                    case(CLI_PARAM_CALL_AUTO_ANSWER_TIME):
                        break;
                    case(CLI_PARAM_POST_TEST_REGISTRATION_TIME):
                        break;
                    case(CLI_PARAM_TEST_MODE_END_DISTANCE):
                        break;
                    case(CLI_PARAM_GARAGE_MODE_END_DISTANCE):
                        break;
                    case(CLI_PARAM_ECALL_TEST_NUMBER):
                        tmp_str = get_ECALL_TEST_NUMBER();
                        break;
                    case(CLI_PARAM_GARAGE_MODE_PIN):
                        break;
                    case(CLI_PARAM_INT_MEM_TRANSMIT_INTERVAL):
                        break;
                    case(CLI_PARAM_INT_MEM_TRANSMIT_ATTEMPTS):
                        break;
                    case(CLI_PARAM_CRASH_SIGNAL_INTERNAL):
                        break;
                    case(CLI_PARAM_CRASH_SIGNAL_EXTERNAL):
                        break;
                    case(CLI_PARAM_ASI15_TRESHOLD):
                        tmp_f = get_ASI15_TRESHOLD();
                        break;
                    case(CLI_PARAM_ECALL_MODE_PIN):
                        break;
                    case(CLI_PARAM_SOS_BUTTON_TIME):
                        break;
                    case(CLI_PARAM_CCFT):
                        break;
                    case(CLI_PARAM_MSD_MAX_TRANSMISSION_TIME):
                        break;
                    case(CLI_PARAM_NAD_DEREGISTRATION_TIME):
                        break;
                    case(CLI_PARAM_ECALL_NO_AUTOMATIC_TRIGGERING):
                        break;
                    case(CLI_PARAM_ECALL_DIAL_DURATION):
                        break;
                    case(CLI_PARAM_ECALL_AUTO_DIAL_ATTEMPTS):
                        break;
                    case(CLI_PARAM_ECALL_MANUAL_DIAL_ATTEMPTS):
                        break;
                    case(CLI_PARAM_ECALL_MANUAL_CAN_CANCEL):
                        break;
                    case(CLI_PARAM_ECALL_SMS_FALLBACK_NUMBER):
                        break;
                    case(CLI_PARAM_TEST_REGISTRATION_PERIOD):
                        break;
                    case(CLI_PARAM_IGNITION_OFF_FOLLOW_UP_TIME1):
                        break;
                    case(CLI_PARAM_IGNITION_OFF_FOLLOW_UP_TIME2):
                        break;
                    case(CLI_PARAM_CRASH_RECORD_TIME):
                        break;
                    case(CLI_PARAM_CRASH_RECORD_RESOLUTION):
                        break;
                    case(CLI_PARAM_CRASH_PRE_RECORD_TIME):
                        break;
                    case(CLI_PARAM_CRASH_PRE_RECORD_RESOLUTION):
                        break;
                    case(CLI_PARAM_GNSS_POWER_OFF_TIME):
                        break;
                    case(CLI_PARAM_GNSS_DATA_RATE):
                        break;
                    case(CLI_PARAM_GNSS_MIN_ELEVATION):
                        break;
                    case(CLI_PARAM_VIN):
                        break;
                    case(CLI_PARAM_VEHICLE_TYPE):
                        break;
                    case(CLI_PARAM_VEHICLE_PROPULSION_STORAGE_TYPE):
                        break;
                    case(CLI_PARAM_ICCID_ECALL):
                        break;
                    case(CLI_PARAM_IMEI_ECALL):
                        break;
                    case(CLI_PARAM_ESN):
                        break;
                    case(CLI_PARAM_GNSS_FIX):
                        break;
                    case(CLI_PARAM_GSM_REG_STATUS):
                        break;
                    case(CLI_PARAM_SOS_BUTTON_INPUT_STATUS):
                        break;
                    case(CLI_PARAM_FUNC_BUTTON_INPUT_STATUS):
                        break;
                    case(CLI_PARAM_IGN_INPUT_STATUS):
                        break;
                    case(CLI_PARAM_ECALL_MODE_PIN_OUPUT_STATUS):
                        break;
                    case(CLI_PARAM_GARAGE_MODE_PIN_OUTPUT_STATUS):
                        break;
                    case(CLI_PARAM_SOS_INDICATOR_OUTPUT_STATUS):
                        break;
                    case(CLI_PARAM_ECALL_EMERGENCY_DEBUG):
                        break;
                    case(CLI_PARAM_ECALL_EMERGENCY_DEBUG_NUMBER):
                        break;
                    case(CLI_PARAM_DEBUG_OUTPUT_ECALL):
                        break;
                    case(CLI_PARAM_ECALL_ON):
                        break;
                    case(CLI_PARAM_ECALL_SPEAKER_VOLUME):
                        break;
                    case(CLI_PARAM_ECALL_MICROPHONE_LEVEL):
                        break;
                    case(CLI_PARAM_FOTA_STATUS):
                        break;
                    case(CLI_PARAM_CRASH_TURNOVER_THRESHOLD):
                        break;
                    case(CLI_PARAM_CRASH_TURNOVER_DURATION):
                        break;
                    case(CLI_PARAM_MOUNT_TYPE):
                        break;
                    case(CLI_PARAM_CRASH_INPUT_PIN):
                        break;
                    case(CLI_PARAM_NUMBER_TROUBLE_CODE):
                        break;
                    case(CLI_PARAM_SIGNAL_STRENGTH):
                        break;
                    case(CLI_PARAM_SUPPLY_VOLTAGE):
                        break;
                    case(CLI_PARAM_BOOT_VERSION):
                        break;
                    case(CLI_PARAM_CLAIBRATION_NUMBER):
                        break;
                    case(CLI_PARAM_MODULE_NUMBER):
                        break;
                    case(CLI_PARAM_STRATEGY_NUMBER):
                        break;
                    case(CLI_PARAM_HARDWARE_NUMBER):
                        break;
                    case(CLI_PARAM_MODEM_SOFTWARE_NUMBER):
                        break;
                    case(CLI_PARAM_IMU_SOFTWARE_NUMBER):
                        break;
                    case(CLI_PARAM_MSD_NUMBER):
                        break;
                    case(CLI_PARAM_LAST_MSD):
                        break;
                    case(CLI_PARAM_CURRENT_GNSS_LAT):
                        break;
                    case(CLI_PARAM_CURRENT_GNSS_LONG):
                        break;
                    case(CLI_PARAM_CURRENT_GNSS_TIME):
                        break;
                    default:
                        break;     
                }
                goto OK_response;
                break;
            }
        }
        err_name = CLI_RESP_ERR_PARAMETER_NOT_EXIST;
        goto NOK_response;
    }

NOK_response:
    printf("READ NOK %s\r\n",CLI_resp_err_names[err_name]);
    return 0;

OK_response:
    switch(var_type){
        case(CLI_PARAM_TYPE_ENUM):
            printf("READ OK %s %s\r\n", CLI_cmd_param_names[i], tmp_str);
            break;
        case(CLI_PARAM_TYPE_INT):
            printf("READ OK %s %d\r\n", CLI_cmd_param_names[i], tmp_i);
            break;
        case(CLI_PARAM_TYPE_FLOAT):
            printf("READ OK %s %.1f\r\n", CLI_cmd_param_names[i], tmp_f);
            break;
        case(CLI_PARAM_TYPE_STRING):
            printf("READ OK %s %s\r\n", CLI_cmd_param_names[i], tmp_str);
            break;
        case(CLI_PARAM_TYPE_BOOL):
            printf("READ OK %s %s\r\n", CLI_cmd_param_names[i], tmp_str);
            break;
        default:
            break;
    }
    return 0;
} 

int write_set(int argc, char **argv) {
    int i=-1;
    int var_type = -1;

    int tmp_i = 0;
    float tmp_f = 0;
    char *tmp_str = NULL;

    CLI_resp_err err_name = CLI_RESP_ERR_UNKNOWN_ERROR;

    int nerrors = arg_parse(argc, argv, (void **) &write_args);
    if (nerrors != 0) {
        err_name = CLI_RESP_ERR_COMMAND_INCORRECT;
        goto NOK_response;
    }

    //GET PARAMETER 
    if (write_args.parameter->count) {
        char param_name[32] = {0};
        const char *src = write_args.parameter->sval[0];
        size_t len = strlen(src);
        if (len > sizeof(param_name)) {
            len = sizeof(param_name);
        }
        memcpy(param_name, src, len);

        for (i=0; i<parameters_number; i++){  //j==0 [HARD RESET], j==1 [SOFT RESET]
            if (strstr(param_name, CLI_cmd_param_names[i]) != NULL) {
                var_type = CLI_param_value_type[i];
                goto next;
            }
        }
        err_name = CLI_RESP_ERR_PARAMETER_NOT_EXIST;
        goto NOK_response;
    }

next:
    //GET VALUE
    if (write_args.value->count) {
        char value_holder[64] = {0};
        const char *src_val = write_args.value->sval[0];
        size_t len = strlen(src_val);
        if (len > sizeof(value_holder)) {
            len = sizeof(value_holder);
        }
        memcpy(value_holder, src_val, len);
    
        switch(var_type){
        case(CLI_PARAM_TYPE_ENUM):
            tmp_str = value_holder;
            break;
        case(CLI_PARAM_TYPE_INT):
            if(0!=CLI_get_value(value_holder,var_type,&tmp_f, &tmp_i)){
                err_name = CLI_RESP_ERR_VALUE_FORMAT_NOT_VALID;
                goto NOK_response;
            }
            break;
        case(CLI_PARAM_TYPE_FLOAT):
            if(0!=CLI_get_value(value_holder,var_type,&tmp_f, &tmp_i)){
                err_name = CLI_RESP_ERR_VALUE_FORMAT_NOT_VALID;
                goto NOK_response;
            }
            break;
        case(CLI_PARAM_TYPE_STRING):
            tmp_str = value_holder;
            break;
        case(CLI_PARAM_TYPE_BOOL):
            if(0!=CLI_get_value(value_holder,var_type,&tmp_f, &tmp_i)){
                err_name = CLI_RESP_ERR_VALUE_FORMAT_NOT_VALID;
                goto NOK_response;
            }
            break;
        default:
            break;
    }

    //PERFORM WRITE OPERATIONS
    switch(i){
        case(CLI_PARAM_RADIO_MUTE_DELAY):
            set_RADIO_MUTE_DELAY(tmp_i);
            tmp_i = get_RADIO_MUTE_DELAY();
            break;
        case(CLI_PARAM_RADIO_UNMUTE_DELAY):
            break;
        case(CLI_PARAM_CALL_AUTO_ANSWER_TIME):
            break;
        case(CLI_PARAM_POST_TEST_REGISTRATION_TIME):
            break;
        case(CLI_PARAM_TEST_MODE_END_DISTANCE):
            break;
        case(CLI_PARAM_GARAGE_MODE_END_DISTANCE):
            break;
        case(CLI_PARAM_ECALL_TEST_NUMBER):
            set_ECALL_TEST_NUMBER(tmp_str);
            tmp_str = get_ECALL_TEST_NUMBER();
            break;
        case(CLI_PARAM_GARAGE_MODE_PIN):
            break;
        case(CLI_PARAM_INT_MEM_TRANSMIT_INTERVAL):
            break;
        case(CLI_PARAM_INT_MEM_TRANSMIT_ATTEMPTS):
            break;
        case(CLI_PARAM_CRASH_SIGNAL_INTERNAL):
            break;
        case(CLI_PARAM_CRASH_SIGNAL_EXTERNAL):
            break;
        case(CLI_PARAM_ASI15_TRESHOLD):
            set_ASI15_TRESHOLD(tmp_f);
            tmp_f = get_ASI15_TRESHOLD();
            break;
        case(CLI_PARAM_ECALL_MODE_PIN):
            break;
        case(CLI_PARAM_SOS_BUTTON_TIME):
            break;
        case(CLI_PARAM_CCFT):
            break;
        case(CLI_PARAM_MSD_MAX_TRANSMISSION_TIME):
            break;
        case(CLI_PARAM_NAD_DEREGISTRATION_TIME):
            break;
        case(CLI_PARAM_ECALL_NO_AUTOMATIC_TRIGGERING):
            break;
        case(CLI_PARAM_ECALL_DIAL_DURATION):
            break;
        case(CLI_PARAM_ECALL_AUTO_DIAL_ATTEMPTS):
            break;
        case(CLI_PARAM_ECALL_MANUAL_DIAL_ATTEMPTS):
            break;
        case(CLI_PARAM_ECALL_MANUAL_CAN_CANCEL):
            break;
        case(CLI_PARAM_ECALL_SMS_FALLBACK_NUMBER):
            break;
        case(CLI_PARAM_TEST_REGISTRATION_PERIOD):
            break;
        case(CLI_PARAM_IGNITION_OFF_FOLLOW_UP_TIME1):
            break;
        case(CLI_PARAM_IGNITION_OFF_FOLLOW_UP_TIME2):
            break;
        case(CLI_PARAM_CRASH_RECORD_TIME):
            break;
        case(CLI_PARAM_CRASH_RECORD_RESOLUTION):
            break;
        case(CLI_PARAM_CRASH_PRE_RECORD_TIME):
            break;
        case(CLI_PARAM_CRASH_PRE_RECORD_RESOLUTION):
            break;
        case(CLI_PARAM_GNSS_POWER_OFF_TIME):
            break;
        case(CLI_PARAM_GNSS_DATA_RATE):
            break;
        case(CLI_PARAM_GNSS_MIN_ELEVATION):
            break;
        case(CLI_PARAM_VIN):
            break;
        case(CLI_PARAM_VEHICLE_TYPE):
            break;
        case(CLI_PARAM_VEHICLE_PROPULSION_STORAGE_TYPE):
            break;
        case(CLI_PARAM_ICCID_ECALL):
            break;
        case(CLI_PARAM_IMEI_ECALL):
            break;
        case(CLI_PARAM_ESN):
            break;
        case(CLI_PARAM_GNSS_FIX):
            break;
        case(CLI_PARAM_GSM_REG_STATUS):
            break;
        case(CLI_PARAM_SOS_BUTTON_INPUT_STATUS):
            break;
        case(CLI_PARAM_FUNC_BUTTON_INPUT_STATUS):
            break;
        case(CLI_PARAM_IGN_INPUT_STATUS):
            break;
        case(CLI_PARAM_ECALL_MODE_PIN_OUPUT_STATUS):
            break;
        case(CLI_PARAM_GARAGE_MODE_PIN_OUTPUT_STATUS):
            break;
        case(CLI_PARAM_SOS_INDICATOR_OUTPUT_STATUS):
            break;
        case(CLI_PARAM_ECALL_EMERGENCY_DEBUG):
            break;
        case(CLI_PARAM_ECALL_EMERGENCY_DEBUG_NUMBER):
            break;
        case(CLI_PARAM_DEBUG_OUTPUT_ECALL):
            break;
        case(CLI_PARAM_ECALL_ON):
            break;
        case(CLI_PARAM_ECALL_SPEAKER_VOLUME):
            break;
        case(CLI_PARAM_ECALL_MICROPHONE_LEVEL):
            break;
        case(CLI_PARAM_FOTA_STATUS):
            break;
        case(CLI_PARAM_CRASH_TURNOVER_THRESHOLD):
            break;
        case(CLI_PARAM_CRASH_TURNOVER_DURATION):
            break;
        case(CLI_PARAM_MOUNT_TYPE):
            break;
        case(CLI_PARAM_CRASH_INPUT_PIN):
            break;
        case(CLI_PARAM_NUMBER_TROUBLE_CODE):
            break;
        case(CLI_PARAM_SIGNAL_STRENGTH):
            break;
        case(CLI_PARAM_SUPPLY_VOLTAGE):
            break;
        case(CLI_PARAM_BOOT_VERSION):
            break;
        case(CLI_PARAM_CLAIBRATION_NUMBER):
            break;
        case(CLI_PARAM_MODULE_NUMBER):
            break;
        case(CLI_PARAM_STRATEGY_NUMBER):
            break;
        case(CLI_PARAM_HARDWARE_NUMBER):
            break;
        case(CLI_PARAM_MODEM_SOFTWARE_NUMBER):
            break;
        case(CLI_PARAM_IMU_SOFTWARE_NUMBER):
            break;
        case(CLI_PARAM_MSD_NUMBER):
            break;
        case(CLI_PARAM_LAST_MSD):
            break;
        case(CLI_PARAM_CURRENT_GNSS_LAT):
            break;
        case(CLI_PARAM_CURRENT_GNSS_LONG):
            break;
        case(CLI_PARAM_CURRENT_GNSS_TIME):
            break;
        default:
            break;     
    }
    goto OK_response;
    }

NOK_response:
    printf("WRITE NOK %s\r\n",CLI_resp_err_names[err_name]);
    return 0;

OK_response:
    switch(var_type){
        case(CLI_PARAM_TYPE_ENUM):
            printf("WRITE OK %s %s\r\n", CLI_cmd_param_names[i], tmp_str);
            break;
        case(CLI_PARAM_TYPE_INT):
            printf("WRITE OK %s %d\r\n", CLI_cmd_param_names[i], tmp_i);
            break;
        case(CLI_PARAM_TYPE_FLOAT):
            printf("WRITE OK %s %.1f\r\n", CLI_cmd_param_names[i], tmp_f);
            break;
        case(CLI_PARAM_TYPE_STRING):
            printf("WRITE OK %s %s\r\n", CLI_cmd_param_names[i], tmp_str);
            break;
        case(CLI_PARAM_TYPE_BOOL):
            printf("WRITE OK %s %s\r\n", CLI_cmd_param_names[i], tmp_str);
            break;
        default:
            break;
    }
    return 0;
} 

static void register_reset_set(void)
{
    int num_args = 1;
    reset_args.opt1 = arg_str1(NULL, NULL, NULL, "HARD or SOFT" );
    reset_args.opt2 = arg_str0(NULL, NULL, NULL, "RESP(by default) or NO_RESP" );
    reset_args.end = arg_end(num_args);

    const esp_console_cmd_t cmd = {
        .command = "RESET",
        .help = "RESET SERVICE",
        .hint = NULL,
        .func = &reset_set,
        .argtable = &reset_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_read_set(void)
{
    int num_args = 1;
    read_args.parameter = arg_str1(NULL, NULL, NULL, "PARAMETER NAME" );
    read_args.end = arg_end(num_args);

    const esp_console_cmd_t cmd = {
        .command = "READ",
        .help = "READ SERVICE",
        .hint = NULL,
        .func = &read_set,
        .argtable = &read_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_write_set(void)
{
    int num_args = 1;
    write_args.parameter = arg_str1(NULL, NULL, NULL, "PARAMETER NAME" );
    write_args.value = arg_str1(NULL, NULL, NULL, "PARAMETER VALUE" );
    write_args.end = arg_end(num_args);

    const esp_console_cmd_t cmd = {
        .command = "WRITE",
        .help = "WRITE SERVICE",
        .hint = NULL,
        .func = &write_set,
        .argtable = &write_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

int float_validation(const char *str, float* f_val)
{
    int len;
    float dummy = 0.0;
    if (sscanf(str, "%f %n", &dummy, &len) == 1 && len == (int)strlen(str)) {
        *f_val = dummy;
        return 0;
    }else{
        return 1;
    }
}

int CLI_get_value(char *value_string, int datatype, float* f_val, int * int_val){
    //VALUE PARSING
    switch(datatype){
        case CLI_PARAM_TYPE_INT:
            //NO INTEGER VALIDATION
            *int_val = atoi(value_string);
            return 0;
            
            break;
        case CLI_PARAM_TYPE_FLOAT:
            //FLOAT VALIDATION
            if(0 == float_validation(value_string, f_val)){
                return 0;
            } else {
                return 1;
            }
            break;
        
        case CLI_PARAM_TYPE_BOOL:
            //BOOL VALIDATION
            for(int i=0; i<2;i++){
                if (strstr(value_string, BOOL_names[i]) != NULL) {    
                    *int_val = i;
                    return 0;
                }
            }
            return 1;
            break;
        default:
            break;
    }
    return 1;
}

/* Console command history can be stored to and loaded from a file.
 * The easiest way to do this is to use FATFS filesystem on top of
 * wear_levelling library.
 */
#if CONFIG_CONSOLE_STORE_HISTORY

#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}
#endif // CONFIG_STORE_HISTORY

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH;

    initialize_nvs();

#if CONFIG_CONSOLE_STORE_HISTORY
    initialize_filesystem();
    repl_config.history_save_path = HISTORY_PATH;
    ESP_LOGI(TAG, "Command history enabled");
#else
    ESP_LOGI(TAG, "Command history disabled");
#endif

    /* Register commands */
    esp_console_register_help_command();
    register_system();
    register_wifi();
    register_nvs();

    CLI_service_init();

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

#else
#error Unsupported console type
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void CLI_service_init(){
    get_parameters_count();

    //commands register
    register_reset_set();
    register_read_set();
    register_write_set();
}

//TEST RESETS
void hard_reset(){
    printf("HARD_RESET_EXECUTION\r\n");
};
void soft_reset(){
     printf("SOFT_RESET_EXECUTION\r\n");
};

//TEST INTERGER VALUE
int get_RADIO_MUTE_DELAY() {
    return test_RADIO_MUTE_DELAY;
};
int set_RADIO_MUTE_DELAY(int val) {
    test_RADIO_MUTE_DELAY = val;
    return test_RADIO_MUTE_DELAY;
};

//TEST STRING VALUE
char* get_ECALL_TEST_NUMBER() {
    return test_ECALL_TEST_NUMBER;
};
void set_ECALL_TEST_NUMBER(char *str_val) {
    sprintf(test_ECALL_TEST_NUMBER,str_val);
};

//TEST FLOAT VALUE
float get_ASI15_TRESHOLD(){
    return test_ASI15_TRESHOLD;
};
float set_ASI15_TRESHOLD(float val){
    test_ASI15_TRESHOLD = val;
    return test_ASI15_TRESHOLD;
};