/**
 * @file NukiUtills.cpp
 * Implementation of generic/helper functions
 *
 * Created on: 2022
 * License: GNU GENERAL PUBLIC LICENSE (see LICENSE)
 *
 * This library implements the communication from an ESP32 via BLE to a Nuki smart lock.
 * Based on the Nuki Smart Lock API V2.2.1
 * https://developer.nuki.io/page/nuki-smart-lock-api-2/2/
 *
 */

#include "NukiOpenerUtils.h"

#include <bitset>
#include <list>
#include <cstdint>
#include <cstring>
#include <string>

#include "esp_log.h"


namespace NukiOpener {
  
void cmdResultToString(const CmdResult state, char* str) {
  switch (state) {
    case CmdResult::Success:
      strcpy(str, "success");
      break;
    case CmdResult::Failed:
      strcpy(str, "failed");
      break;
    case CmdResult::TimeOut:
      strcpy(str, "timeOut");
      break;
    case CmdResult::Working:
      strcpy(str, "working");
      break;
    case CmdResult::NotPaired:
      strcpy(str, "notPaired");
      break;
    case CmdResult::Error:
      strcpy(str, "error");
      break;
    default:
      strcpy(str, "undefined");
      break;
  }
}

void logOpenerErrorCode(uint8_t errorCode, bool debug) {

  switch (errorCode) {
    case (uint8_t)ErrorCode::ERROR_BAD_CRC :
      ESP_LOGE("NukiBle.NukiOpener", "ERROR_BAD_CRC");
      break;
    case (uint8_t)ErrorCode::ERROR_BAD_LENGTH :
      ESP_LOGE("NukiBle.NukiOpener", "ERROR_BAD_LENGTH");
      break;
    case (uint8_t)ErrorCode::ERROR_UNKNOWN :
      ESP_LOGE("NukiBle.NukiOpener", "ERROR_UNKNOWN");
      break;
    case (uint8_t)ErrorCode::P_ERROR_NOT_PAIRING :
      ESP_LOGE("NukiBle.NukiOpener", "P_ERROR_NOT_PAIRING");
      break;
    case (uint8_t)ErrorCode::P_ERROR_BAD_AUTHENTICATOR :
      ESP_LOGE("NukiBle.NukiOpener", "P_ERROR_BAD_AUTHENTICATOR");
      break;
    case (uint8_t)ErrorCode::P_ERROR_BAD_PARAMETER :
      ESP_LOGE("NukiBle.NukiOpener", "P_ERROR_BAD_PARAMETER");
      break;
    case (uint8_t)ErrorCode::P_ERROR_MAX_USER :
      ESP_LOGE("NukiBle.NukiOpener", "P_ERROR_MAX_USER");
      break;
    case (uint8_t)ErrorCode::K_ERROR_AUTO_UNLOCK_TOO_RECENT :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_AUTO_UNLOCK_TOO_RECENT");
      break;
    case (uint8_t)ErrorCode::K_ERROR_BAD_NONCE :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_BAD_NONCE");
      break;
    case (uint8_t)ErrorCode::K_ERROR_BAD_PARAMETER :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_BAD_PARAMETER");
      break;
    case (uint8_t)ErrorCode::K_ERROR_BAD_PIN :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_BAD_PIN");
      break;
    case (uint8_t)ErrorCode::K_ERROR_BUSY :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_BUSY");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CANCELED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CANCELED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CLUTCH_FAILURE :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CLUTCH_FAILURE");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CLUTCH_POWER_FAILURE :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CLUTCH_POWER_FAILURE");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CODE_ALREADY_EXISTS :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CODE_ALREADY_EXISTS");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CODE_INVALID :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CODE_INVALID");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CODE_INVALID_TIMEOUT_1 :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CODE_INVALID_TIMEOUT_1");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CODE_INVALID_TIMEOUT_2 :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CODE_INVALID_TIMEOUT_2");
      break;
    case (uint8_t)ErrorCode::K_ERROR_CODE_INVALID_TIMEOUT_3 :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_CODE_INVALID_TIMEOUT_3");
      break;
    case (uint8_t)ErrorCode::K_ERROR_DISABLED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_DISABLED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_FIRMWARE_UPDATE_NEEDED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_FIRMWARE_UPDATE_NEEDED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_INVALID_AUTH_ID :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_INVALID_AUTH_ID");
      break;
    case (uint8_t)ErrorCode::K_ERROR_MOTOR_BLOCKED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_MOTOR_BLOCKED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_MOTOR_LOW_VOLTAGE :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_MOTOR_LOW_VOLTAGE");
      break;
    case (uint8_t)ErrorCode::K_ERROR_MOTOR_POSITION_LIMIT :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_MOTOR_POSITION_LIMIT");
      break;
    case (uint8_t)ErrorCode::K_ERROR_MOTOR_POWER_FAILURE :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_MOTOR_POWER_FAILURE");
      break;
    case (uint8_t)ErrorCode::K_ERROR_MOTOR_TIMEOUT :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_MOTOR_TIMEOUT");
      break;
    case (uint8_t)ErrorCode::K_ERROR_NOT_AUTHORIZED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_NOT_AUTHORIZED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_NOT_CALIBRATED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_NOT_CALIBRATED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_POSITION_UNKNOWN :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_POSITION_UNKNOWN");
      break;
    case (uint8_t)ErrorCode::K_ERROR_REMOTE_NOT_ALLOWED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_REMOTE_NOT_ALLOWED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_TIME_NOT_ALLOWED :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_TIME_NOT_ALLOWED");
      break;
    case (uint8_t)ErrorCode::K_ERROR_TOO_MANY_ENTRIES :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_TOO_MANY_ENTRIES");
      break;
    case (uint8_t)ErrorCode::K_ERROR_TOO_MANY_PIN_ATTEMPTS :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_TOO_MANY_PIN_ATTEMPTS");
      break;
    case (uint8_t)ErrorCode::K_ERROR_VOLTAGE_TOO_LOW :
      ESP_LOGE("NukiBle.NukiOpener", "K_ERROR_VOLTAGE_TOO_LOW");
      break;
    default:
      ESP_LOGE("NukiBle.NukiOpener", "UNDEFINED ERROR");
  }
}

void logConfig(Config config, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "nukiId :%d", (unsigned int)config.nukiId);
    ESP_LOGD("NukiBle.NukiOpener", "name :%s", (const char*)config.name);
    ESP_LOGD("NukiBle.NukiOpener", "capabilities :%d", (unsigned int)config.capabilities);
    ESP_LOGD("NukiBle.NukiOpener", "latitude :%f", (const float)config.latitude);
    ESP_LOGD("NukiBle.NukiOpener", "longitude :%f", (const float)config.longitude);
    ESP_LOGD("NukiBle.NukiOpener", "pairingEnabled :%d", (unsigned int)config.pairingEnabled);
    ESP_LOGD("NukiBle.NukiOpener", "buttonEnabled :%d", (unsigned int)config.buttonEnabled);
    ESP_LOGD("NukiBle.NukiOpener", "currentTime Year :%d", (unsigned int)config.currentTimeYear);
    ESP_LOGD("NukiBle.NukiOpener", "currentTime Month :%d", (unsigned int)config.currentTimeMonth);
    ESP_LOGD("NukiBle.NukiOpener", "currentTime Day :%d", (unsigned int)config.currentTimeDay);
    ESP_LOGD("NukiBle.NukiOpener", "currentTime Hour :%d", (unsigned int)config.currentTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "currentTime Minute :%d", (unsigned int)config.currentTimeMinute);
    ESP_LOGD("NukiBle.NukiOpener", "currentTime Second :%d", (unsigned int)config.currentTimeSecond);
    ESP_LOGD("NukiBle.NukiOpener", "timeZoneOffset :%d", (unsigned int)config.timeZoneOffset);
    ESP_LOGD("NukiBle.NukiOpener", "dstMode :%d", (unsigned int)config.dstMode);
    ESP_LOGD("NukiBle.NukiOpener", "hasFob :%d", (unsigned int)config.hasFob);
    ESP_LOGD("NukiBle.NukiOpener", "fobAction1 :%d", (unsigned int)config.fobAction1);
    ESP_LOGD("NukiBle.NukiOpener", "fobAction2 :%d", (unsigned int)config.fobAction2);
    ESP_LOGD("NukiBle.NukiOpener", "fobAction3 :%d", (unsigned int)config.fobAction3);
    ESP_LOGD("NukiBle.NukiOpener", "operatingMode :%d", (unsigned int)config.operatingMode);    
    ESP_LOGD("NukiBle.NukiOpener", "advertisingMode :%d", (unsigned int)config.advertisingMode);
    ESP_LOGD("NukiBle.NukiOpener", "hasKeypad :%d", (unsigned int)config.hasKeypad);
    ESP_LOGD("NukiBle.NukiOpener", "firmwareVersion :%d.%d.%d", config.firmwareVersion[0], config.firmwareVersion[1], config.firmwareVersion[2]);
    ESP_LOGD("NukiBle.NukiOpener", "hardwareRevision :%d.%d", config.hardwareRevision[0], config.hardwareRevision[1]);
    ESP_LOGD("NukiBle.NukiOpener", "timeZoneId :%d", (unsigned int)config.timeZoneId);
    ESP_LOGD("NukiBle.NukiOpener", "hasKeypadV2 :%d", (unsigned int)config.hasKeypadV2);
  }
}

void logNewConfig(NewConfig newConfig, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "name: %s", (const char*)newConfig.name);
    ESP_LOGD("NukiBle.NukiOpener", "latitude: %f", (const float)newConfig.latitude);
    ESP_LOGD("NukiBle.NukiOpener", "longitude: %f", (const float)newConfig.longitude);
    ESP_LOGD("NukiBle.NukiOpener", "pairingEnabled: %d", (unsigned int)newConfig.pairingEnabled);
    ESP_LOGD("NukiBle.NukiOpener", "buttonEnabled: %d", (unsigned int)newConfig.buttonEnabled);
    ESP_LOGD("NukiBle.NukiOpener", "timeZoneOffset: %d", (unsigned int)newConfig.timeZoneOffset);
    ESP_LOGD("NukiBle.NukiOpener", "dstMode: %d", (unsigned int)newConfig.dstMode);
    ESP_LOGD("NukiBle.NukiOpener", "fobAction1: %d", (unsigned int)newConfig.fobAction1);
    ESP_LOGD("NukiBle.NukiOpener", "fobAction2: %d", (unsigned int)newConfig.fobAction2);
    ESP_LOGD("NukiBle.NukiOpener", "fobAction3: %d", (unsigned int)newConfig.fobAction3);
    ESP_LOGD("NukiBle.NukiOpener", "advertisingMode: %d", (unsigned int)newConfig.advertisingMode);
    ESP_LOGD("NukiBle.NukiOpener", "timeZoneId: %d", (unsigned int)newConfig.timeZoneId);
  }
}

void logNewKeypadEntry(NewKeypadEntry newKeypadEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "code:%d", (unsigned int)newKeypadEntry.code);
    ESP_LOGD("NukiBle.NukiOpener", "name:%s", (const char*)newKeypadEntry.name);
    ESP_LOGD("NukiBle.NukiOpener", "timeLimited:%d", (unsigned int)newKeypadEntry.timeLimited);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromYear:%d", (unsigned int)newKeypadEntry.allowedFromYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMonth:%d", (unsigned int)newKeypadEntry.allowedFromMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromDay:%d", (unsigned int)newKeypadEntry.allowedFromDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromHour:%d", (unsigned int)newKeypadEntry.allowedFromHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMin:%d", (unsigned int)newKeypadEntry.allowedFromMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromSec:%d", (unsigned int)newKeypadEntry.allowedFromSec);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilYear:%d", (unsigned int)newKeypadEntry.allowedUntilYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMonth:%d", (unsigned int)newKeypadEntry.allowedUntilMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilDay:%d", (unsigned int)newKeypadEntry.allowedUntilDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilHour:%d", (unsigned int)newKeypadEntry.allowedUntilHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMin:%d", (unsigned int)newKeypadEntry.allowedUntilMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilSec:%d", (unsigned int)newKeypadEntry.allowedUntilSec);
    ESP_LOGD("NukiBle.NukiOpener", "allowedWeekdays:%d", (unsigned int)newKeypadEntry.allowedWeekdays);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeHour:%d", (unsigned int)newKeypadEntry.allowedFromTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeMin:%d", (unsigned int)newKeypadEntry.allowedFromTimeMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeHour:%d", (unsigned int)newKeypadEntry.allowedUntilTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeMin:%d", (unsigned int)newKeypadEntry.allowedUntilTimeMin);
  }
}

void logKeypadEntry(KeypadEntry keypadEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "codeId:%d", (unsigned int)keypadEntry.codeId);
    ESP_LOGD("NukiBle.NukiOpener", "code:%d", (unsigned int)keypadEntry.code);
    ESP_LOGD("NukiBle.NukiOpener", "name:%s", (const char*)keypadEntry.name);
    ESP_LOGD("NukiBle.NukiOpener", "enabled:%d", (unsigned int)keypadEntry.enabled);
    ESP_LOGD("NukiBle.NukiOpener", "dateCreatedYear:%d", (unsigned int)keypadEntry.dateCreatedYear);
    ESP_LOGD("NukiBle.NukiOpener", "dateCreatedMonth:%d", (unsigned int)keypadEntry.dateCreatedMonth);
    ESP_LOGD("NukiBle.NukiOpener", "dateCreatedDay:%d", (unsigned int)keypadEntry.dateCreatedDay);
    ESP_LOGD("NukiBle.NukiOpener", "dateCreatedHour:%d", (unsigned int)keypadEntry.dateCreatedHour);
    ESP_LOGD("NukiBle.NukiOpener", "dateCreatedMin:%d", (unsigned int)keypadEntry.dateCreatedMin);
    ESP_LOGD("NukiBle.NukiOpener", "dateCreatedSec:%d", (unsigned int)keypadEntry.dateCreatedSec);
    ESP_LOGD("NukiBle.NukiOpener", "dateLastActiveYear:%d", (unsigned int)keypadEntry.dateLastActiveYear);
    ESP_LOGD("NukiBle.NukiOpener", "dateLastActiveMonth:%d", (unsigned int)keypadEntry.dateLastActiveMonth);
    ESP_LOGD("NukiBle.NukiOpener", "dateLastActiveDay:%d", (unsigned int)keypadEntry.dateLastActiveDay);
    ESP_LOGD("NukiBle.NukiOpener", "dateLastActiveHour:%d", (unsigned int)keypadEntry.dateLastActiveHour);
    ESP_LOGD("NukiBle.NukiOpener", "dateLastActiveMin:%d", (unsigned int)keypadEntry.dateLastActiveMin);
    ESP_LOGD("NukiBle.NukiOpener", "dateLastActiveSec:%d", (unsigned int)keypadEntry.dateLastActiveSec);
    ESP_LOGD("NukiBle.NukiOpener", "lockCount:%d", (unsigned int)keypadEntry.lockCount);
    ESP_LOGD("NukiBle.NukiOpener", "timeLimited:%d", (unsigned int)keypadEntry.timeLimited);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromYear:%d", (unsigned int)keypadEntry.allowedFromYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMonth:%d", (unsigned int)keypadEntry.allowedFromMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromDay:%d", (unsigned int)keypadEntry.allowedFromDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromHour:%d", (unsigned int)keypadEntry.allowedFromHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMin:%d", (unsigned int)keypadEntry.allowedFromMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromSec:%d", (unsigned int)keypadEntry.allowedFromSec);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilYear:%d", (unsigned int)keypadEntry.allowedUntilYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMonth:%d", (unsigned int)keypadEntry.allowedUntilMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilDay:%d", (unsigned int)keypadEntry.allowedUntilDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilHour:%d", (unsigned int)keypadEntry.allowedUntilHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMin:%d", (unsigned int)keypadEntry.allowedUntilMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilSec:%d", (unsigned int)keypadEntry.allowedUntilSec);
    ESP_LOGD("NukiBle.NukiOpener", "allowedWeekdays:%d", (unsigned int)keypadEntry.allowedWeekdays);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeHour:%d", (unsigned int)keypadEntry.allowedFromTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeMin:%d", (unsigned int)keypadEntry.allowedFromTimeMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeHour:%d", (unsigned int)keypadEntry.allowedUntilTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeMin:%d", (unsigned int)keypadEntry.allowedUntilTimeMin);
  }
}

void logUpdatedKeypadEntry(UpdatedKeypadEntry updatedKeypadEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "codeId:%d", (unsigned int)updatedKeypadEntry.codeId);
    ESP_LOGD("NukiBle.NukiOpener", "code:%d", (unsigned int)updatedKeypadEntry.code);
    ESP_LOGD("NukiBle.NukiOpener", "name:%s", (const char*)updatedKeypadEntry.name);
    ESP_LOGD("NukiBle.NukiOpener", "enabled:%d", (unsigned int)updatedKeypadEntry.enabled);
    ESP_LOGD("NukiBle.NukiOpener", "timeLimited:%d", (unsigned int)updatedKeypadEntry.timeLimited);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromYear:%d", (unsigned int)updatedKeypadEntry.allowedFromYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMonth:%d", (unsigned int)updatedKeypadEntry.allowedFromMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromDay:%d", (unsigned int)updatedKeypadEntry.allowedFromDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromHour:%d", (unsigned int)updatedKeypadEntry.allowedFromHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMin:%d", (unsigned int)updatedKeypadEntry.allowedFromMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromSec:%d", (unsigned int)updatedKeypadEntry.allowedFromSec);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilYear:%d", (unsigned int)updatedKeypadEntry.allowedUntilYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMonth:%d", (unsigned int)updatedKeypadEntry.allowedUntilMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilDay:%d", (unsigned int)updatedKeypadEntry.allowedUntilDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilHour:%d", (unsigned int)updatedKeypadEntry.allowedUntilHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMin:%d", (unsigned int)updatedKeypadEntry.allowedUntilMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilSec:%d", (unsigned int)updatedKeypadEntry.allowedUntilSec);
    ESP_LOGD("NukiBle.NukiOpener", "allowedWeekdays:%d", (unsigned int)updatedKeypadEntry.allowedWeekdays);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeHour:%d", (unsigned int)updatedKeypadEntry.allowedFromTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeMin:%d", (unsigned int)updatedKeypadEntry.allowedFromTimeMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeHour:%d", (unsigned int)updatedKeypadEntry.allowedUntilTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeMin:%d", (unsigned int)updatedKeypadEntry.allowedUntilTimeMin);
  }
}

void logAuthorizationEntry(AuthorizationEntry authorizationEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "id:%d", (unsigned int)authorizationEntry.authId);
    ESP_LOGD("NukiBle.NukiOpener", "idType:%d", (unsigned int)authorizationEntry.idType);
    ESP_LOGD("NukiBle.NukiOpener", "name:%s", (const char*)authorizationEntry.name);
    ESP_LOGD("NukiBle.NukiOpener", "enabled:%d", (unsigned int)authorizationEntry.enabled);
    ESP_LOGD("NukiBle.NukiOpener", "remoteAllowed:%d", (unsigned int)authorizationEntry.remoteAllowed);
    ESP_LOGD("NukiBle.NukiOpener", "createdYear:%d", (unsigned int)authorizationEntry.createdYear);
    ESP_LOGD("NukiBle.NukiOpener", "createdMonth:%d", (unsigned int)authorizationEntry.createdMonth);
    ESP_LOGD("NukiBle.NukiOpener", "createdDay:%d", (unsigned int)authorizationEntry.createdDay);
    ESP_LOGD("NukiBle.NukiOpener", "createdHour:%d", (unsigned int)authorizationEntry.createdHour);
    ESP_LOGD("NukiBle.NukiOpener", "createdMin:%d", (unsigned int)authorizationEntry.createdMinute);
    ESP_LOGD("NukiBle.NukiOpener", "createdSec:%d", (unsigned int)authorizationEntry.createdSecond);
    ESP_LOGD("NukiBle.NukiOpener", "lastactYear:%d", (unsigned int)authorizationEntry.lastActYear);
    ESP_LOGD("NukiBle.NukiOpener", "lastactMonth:%d", (unsigned int)authorizationEntry.lastActMonth);
    ESP_LOGD("NukiBle.NukiOpener", "lastactDay:%d", (unsigned int)authorizationEntry.lastActDay);
    ESP_LOGD("NukiBle.NukiOpener", "lastactHour:%d", (unsigned int)authorizationEntry.lastActHour);
    ESP_LOGD("NukiBle.NukiOpener", "lastactMin:%d", (unsigned int)authorizationEntry.lastActMinute);
    ESP_LOGD("NukiBle.NukiOpener", "lastactSec:%d", (unsigned int)authorizationEntry.lastActSecond);
    ESP_LOGD("NukiBle.NukiOpener", "lockCount:%d", (unsigned int)authorizationEntry.lockCount);
    ESP_LOGD("NukiBle.NukiOpener", "timeLimited:%d", (unsigned int)authorizationEntry.timeLimited);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromYear:%d", (unsigned int)authorizationEntry.allowedFromYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMonth:%d", (unsigned int)authorizationEntry.allowedFromMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromDay:%d", (unsigned int)authorizationEntry.allowedFromDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromHour:%d", (unsigned int)authorizationEntry.allowedFromHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMin:%d", (unsigned int)authorizationEntry.allowedFromMinute);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromSec:%d", (unsigned int)authorizationEntry.allowedFromSecond);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilYear:%d", (unsigned int)authorizationEntry.allowedUntilYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMonth:%d", (unsigned int)authorizationEntry.allowedUntilMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilDay:%d", (unsigned int)authorizationEntry.allowedUntilDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilHour:%d", (unsigned int)authorizationEntry.allowedUntilHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMin:%d", (unsigned int)authorizationEntry.allowedUntilMinute);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilSec:%d", (unsigned int)authorizationEntry.allowedUntilSecond);
    ESP_LOGD("NukiBle.NukiOpener", "allowedWeekdays:%d", (unsigned int)authorizationEntry.allowedWeekdays);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeHour:%d", (unsigned int)authorizationEntry.allowedFromTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeMin:%d", (unsigned int)authorizationEntry.allowedFromTimeMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeHour:%d", (unsigned int)authorizationEntry.allowedUntilTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeMin:%d", (unsigned int)authorizationEntry.allowedUntilTimeMin);
  }
}

void logNewAuthorizationEntry(NewAuthorizationEntry newAuthorizationEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "name:%s", (const char*)newAuthorizationEntry.name);
    ESP_LOGD("NukiBle.NukiOpener", "idType:%d", (unsigned int)newAuthorizationEntry.idType);
    ESP_LOGD("NukiBle.NukiOpener", "remoteAllowed:%d", (unsigned int)newAuthorizationEntry.remoteAllowed);
    ESP_LOGD("NukiBle.NukiOpener", "timeLimited:%d", (unsigned int)newAuthorizationEntry.timeLimited);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromYear:%d", (unsigned int)newAuthorizationEntry.allowedFromYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMonth:%d", (unsigned int)newAuthorizationEntry.allowedFromMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromDay:%d", (unsigned int)newAuthorizationEntry.allowedFromDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromHour:%d", (unsigned int)newAuthorizationEntry.allowedFromHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMin:%d", (unsigned int)newAuthorizationEntry.allowedFromMinute);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromSec:%d", (unsigned int)newAuthorizationEntry.allowedFromSecond);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilYear:%d", (unsigned int)newAuthorizationEntry.allowedUntilYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMonth:%d", (unsigned int)newAuthorizationEntry.allowedUntilMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilDay:%d", (unsigned int)newAuthorizationEntry.allowedUntilDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilHour:%d", (unsigned int)newAuthorizationEntry.allowedUntilHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMin:%d", (unsigned int)newAuthorizationEntry.allowedUntilMinute);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilSec:%d", (unsigned int)newAuthorizationEntry.allowedUntilSecond);
    ESP_LOGD("NukiBle.NukiOpener", "allowedWeekdays:%d", (unsigned int)newAuthorizationEntry.allowedWeekdays);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeHour:%d", (unsigned int)newAuthorizationEntry.allowedFromTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeMin:%d", (unsigned int)newAuthorizationEntry.allowedFromTimeMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeHour:%d", (unsigned int)newAuthorizationEntry.allowedUntilTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeMin:%d", (unsigned int)newAuthorizationEntry.allowedUntilTimeMin);
  }
}

void logUpdatedAuthorizationEntry(UpdatedAuthorizationEntry updatedAuthorizationEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "id:%d", (unsigned int)updatedAuthorizationEntry.authId);
    ESP_LOGD("NukiBle.NukiOpener", "name:%s", (const char*)updatedAuthorizationEntry.name);
    ESP_LOGD("NukiBle.NukiOpener", "enabled:%d", (unsigned int)updatedAuthorizationEntry.enabled);
    ESP_LOGD("NukiBle.NukiOpener", "remoteAllowed:%d", (unsigned int)updatedAuthorizationEntry.remoteAllowed);
    ESP_LOGD("NukiBle.NukiOpener", "timeLimited:%d", (unsigned int)updatedAuthorizationEntry.timeLimited);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromYear:%d", (unsigned int)updatedAuthorizationEntry.allowedFromYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMonth:%d", (unsigned int)updatedAuthorizationEntry.allowedFromMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromDay:%d", (unsigned int)updatedAuthorizationEntry.allowedFromDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromHour:%d", (unsigned int)updatedAuthorizationEntry.allowedFromHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromMin:%d", (unsigned int)updatedAuthorizationEntry.allowedFromMinute);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromSec:%d", (unsigned int)updatedAuthorizationEntry.allowedFromSecond);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilYear:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilYear);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMonth:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilMonth);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilDay:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilDay);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilHour:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilMin:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilMinute);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilSec:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilSecond);
    ESP_LOGD("NukiBle.NukiOpener", "allowedWeekdays:%d", (unsigned int)updatedAuthorizationEntry.allowedWeekdays);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeHour:%d", (unsigned int)updatedAuthorizationEntry.allowedFromTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedFromTimeMin:%d", (unsigned int)updatedAuthorizationEntry.allowedFromTimeMin);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeHour:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "allowedUntilTimeMin:%d", (unsigned int)updatedAuthorizationEntry.allowedUntilTimeMin);
  }
}

void logNewTimeControlEntry(NewTimeControlEntry newTimeControlEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "weekdays:%d", (unsigned int)newTimeControlEntry.weekdays);
    ESP_LOGD("NukiBle.NukiOpener", "time:%d:%d", (unsigned int)newTimeControlEntry.timeHour, newTimeControlEntry.timeMin);
    ESP_LOGD("NukiBle.NukiOpener", "lockAction:%d", (unsigned int)newTimeControlEntry.lockAction);
  }
}

void logTimeControlEntry(TimeControlEntry timeControlEntry, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "entryId:%d", (unsigned int)timeControlEntry.entryId);
    ESP_LOGD("NukiBle.NukiOpener", "enabled:%d", (unsigned int)timeControlEntry.enabled);
    ESP_LOGD("NukiBle.NukiOpener", "weekdays:%d", (unsigned int)timeControlEntry.weekdays);
    ESP_LOGD("NukiBle.NukiOpener", "time:%d:%d", (unsigned int)timeControlEntry.timeHour, timeControlEntry.timeMin);
    ESP_LOGD("NukiBle.NukiOpener", "lockAction:%d", (unsigned int)timeControlEntry.lockAction);
  }
}

void logCompletionStatus(CompletionStatus completionStatus, bool debug) {
  switch (completionStatus) {
    case CompletionStatus::Busy :
      ESP_LOGD("NukiBle.NukiOpener", "Completion status: busy");
      break;
    case CompletionStatus::Incomplete :
      ESP_LOGD("NukiBle.NukiOpener", "Completion status: incomplete");
      break;
    case CompletionStatus::Canceled :
      ESP_LOGD("NukiBle.NukiOpener", "Completion status: canceled");
      break;
    case CompletionStatus::OtherError :
      ESP_LOGD("NukiBle.NukiOpener", "Completion status: otherError");
      break;
    case CompletionStatus::Success :
      ESP_LOGD("NukiBle.NukiOpener", "Completion status: success");
      break;
    case CompletionStatus::TooRecent :
      ESP_LOGD("NukiBle.NukiOpener", "Completion status: tooRecent");
      break;
    default:
      ESP_LOGW("NukiBle.NukiOpener", "Completion status: unknown");
      break;
  }
}

void logNukiTrigger(Trigger nukiTrigger, bool debug) {
  switch (nukiTrigger) {
    case Trigger::Automatic :
      ESP_LOGD("NukiBle.NukiOpener", "Trigger: automatic");
      break;
    case Trigger::Button :
      ESP_LOGD("NukiBle.NukiOpener", "Trigger: button");
      break;
    case Trigger::Manual :
      ESP_LOGD("NukiBle.NukiOpener", "Trigger: manual");
      break;
    case Trigger::System :
      ESP_LOGD("NukiBle.NukiOpener", "Trigger: system");
      break;
    default:
      ESP_LOGW("NukiBle.NukiOpener", "Trigger: unknown");
      break;
  }
}

void logLockAction(LockAction lockAction, bool debug) {
  switch (lockAction) {
    case LockAction::ActivateRTO :
      ESP_LOGD("NukiBle.NukiOpener", "action: ActivateRTO");
      break;
    case LockAction::DeactivateRTO :
      ESP_LOGD("NukiBle.NukiOpener", "action: DeactivateRTO");
      break;
    case LockAction::ElectricStrikeActuation :
      ESP_LOGD("NukiBle.NukiOpener", "action: ElectricStrikeActuation");
      break;
    case LockAction::ActivateCM :
      ESP_LOGD("NukiBle.NukiOpener", "action: ActivateCM");
      break;
    case LockAction::DeactivateCM :
      ESP_LOGD("NukiBle.NukiOpener", "action: DeactivateCM");
      break;
    case LockAction::FobAction1 :
      ESP_LOGD("NukiBle.NukiOpener", "action: autoLock");
      break;
    case LockAction::FobAction2 :
      ESP_LOGD("NukiBle.NukiOpener", "action: automatic");
      break;
    case LockAction::FobAction3 :
      ESP_LOGD("NukiBle.NukiOpener", "action: button");
      break;
    default:
      ESP_LOGW("NukiBle.NukiOpener", "action: unknown");
      break;
  }
}


void logKeyturnerState(OpenerState keyTurnerState, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "nukiState: %02x", (unsigned int)keyTurnerState.nukiState);
    ESP_LOGD("NukiBle.NukiOpener", "lockState: %d", (unsigned int)keyTurnerState.lockState);
    logNukiTrigger(keyTurnerState.trigger, debug);
    ESP_LOGD("NukiBle.NukiOpener", "currentTimeYear: %d", (unsigned int)keyTurnerState.currentTimeYear);
    ESP_LOGD("NukiBle.NukiOpener", "currentTimeMonth: %d", (unsigned int)keyTurnerState.currentTimeMonth);
    ESP_LOGD("NukiBle.NukiOpener", "currentTimeDay: %d", (unsigned int)keyTurnerState.currentTimeDay);
    ESP_LOGD("NukiBle.NukiOpener", "currentTimeHour: %d", (unsigned int)keyTurnerState.currentTimeHour);
    ESP_LOGD("NukiBle.NukiOpener", "currentTimeMinute: %d", (unsigned int)keyTurnerState.currentTimeMinute);
    ESP_LOGD("NukiBle.NukiOpener", "currentTimeSecond: %d", (unsigned int)keyTurnerState.currentTimeSecond);
    ESP_LOGD("NukiBle.NukiOpener", "timeZoneOffset: %d", (unsigned int)keyTurnerState.timeZoneOffset);
    ESP_LOGD("NukiBle.NukiOpener", "criticalBatteryState composed value: %d", (unsigned int)keyTurnerState.criticalBatteryState);
    ESP_LOGD("NukiBle.NukiOpener", "configUpdateCount: %d", (unsigned int)keyTurnerState.configUpdateCount);
    logLockAction((LockAction)keyTurnerState.lastLockAction, debug);
    ESP_LOGD("NukiBle.NukiOpener", "lastLockActionTrigger: %d", (unsigned int)keyTurnerState.lastLockActionTrigger);
    logCompletionStatus(keyTurnerState.lastLockActionCompletionStatus, debug);
    ESP_LOGD("NukiBle.NukiOpener", "Keypad bat critical feature supported: %d", (unsigned int)(((unsigned int)keyTurnerState.accessoryBatteryState & 1) == 1 ? 1 : 0));
    ESP_LOGD("NukiBle.NukiOpener", "Keypad Battery Critical: %d", (unsigned int)(((unsigned int)keyTurnerState.accessoryBatteryState & 3) == 3 ? 1 : 0));  
    }
}

void logBatteryReport(BatteryReport batteryReport, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "batteryDrain:%d", (unsigned int)batteryReport.batteryDrain);
    ESP_LOGD("NukiBle.NukiOpener", "batteryVoltage:%d", (unsigned int)batteryReport.batteryVoltage);
    ESP_LOGD("NukiBle.NukiOpener", "criticalBatteryState:%d", (unsigned int)batteryReport.criticalBatteryState);
    ESP_LOGD("NukiBle.NukiOpener", "lockAction:%d", (unsigned int)batteryReport.lockAction);
    ESP_LOGD("NukiBle.NukiOpener", "startVoltage:%d", (unsigned int)batteryReport.startVoltage);
    ESP_LOGD("NukiBle.NukiOpener", "lowestVoltage:%d", (unsigned int)batteryReport.lowestVoltage);
    ESP_LOGD("NukiBle.NukiOpener", "lockDistance:%d", (unsigned int)batteryReport.lockDistance);
    ESP_LOGD("NukiBle.NukiOpener", "startTemperature:%d", (unsigned int)batteryReport.startTemperature);
    ESP_LOGD("NukiBle.NukiOpener", "maxTurnCurrent:%d", (unsigned int)batteryReport.maxTurnCurrent);
    ESP_LOGD("NukiBle.NukiOpener", "batteryResistance:%d", (unsigned int)batteryReport.batteryResistance);
  }
}

void logLogEntry(LogEntry logEntry, bool debug) {
  ESP_LOGD("NukiBle.NukiOpener", "[%lu] type: %u authId: %lu name: %s %d-%d-%d %d:%d:%d ", logEntry.index, (uint8_t)logEntry.loggingType, logEntry.authId, logEntry.name, logEntry.timeStampYear, logEntry.timeStampMonth, logEntry.timeStampDay, logEntry.timeStampHour, logEntry.timeStampMinute, logEntry.timeStampSecond);

  switch (logEntry.loggingType) {
    case LoggingType::LoggingEnabled: {
      ESP_LOGD("NukiBle.NukiOpener", "Logging enabled: %d", (unsigned int)logEntry.data[0]);
      break;
    }
    case LoggingType::LockAction:
    case LoggingType::Calibration:
    case LoggingType::KeypadAction: {
      logLockAction((LockAction)logEntry.data[0], debug);
      ESP_LOGD("NukiBle.NukiOpener", "Source: %d", (unsigned int)logEntry.data[1]);
      logCompletionStatus((CompletionStatus)logEntry.data[2], debug);
      uint16_t codeId = 0;
      memcpy(&codeId, &logEntry.data[3], 2);
      ESP_LOGD("NukiBle.NukiOpener", "Code id: %d", (unsigned int)codeId);
      break;
    }
    case LoggingType::DoorbellRecognition: {
      // TODO
      break;
    }
    default:
      ESP_LOGW("NukiBle.NukiOpener", "Unknown logging type");
      break;
  }
}

void logAdvancedConfig(AdvancedConfig advancedConfig, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "singleButtonPressAction :%d", (unsigned int)advancedConfig.singleButtonPressAction);
    ESP_LOGD("NukiBle.NukiOpener", "doubleButtonPressAction :%d", (unsigned int)(unsigned int)advancedConfig.doubleButtonPressAction);
    ESP_LOGD("NukiBle.NukiOpener", "batteryType :%d", (unsigned int)advancedConfig.batteryType);
    ESP_LOGD("NukiBle.NukiOpener", "automaticBatteryTypeDetection :%d", (unsigned int)advancedConfig.automaticBatteryTypeDetection);
  }
}

void logNewAdvancedConfig(NewAdvancedConfig newAdvancedConfig, bool debug) {
  if (debug) {
    ESP_LOGD("NukiBle.NukiOpener", "singleButtonPressAction :%d", (unsigned int)newAdvancedConfig.singleButtonPressAction);
    ESP_LOGD("NukiBle.NukiOpener", "doubleButtonPressAction :%d", (unsigned int)newAdvancedConfig.doubleButtonPressAction);
    ESP_LOGD("NukiBle.NukiOpener", "batteryType :%d", (unsigned int)newAdvancedConfig.batteryType);
    ESP_LOGD("NukiBle.NukiOpener", "automaticBatteryTypeDetection :%d", (unsigned int)newAdvancedConfig.automaticBatteryTypeDetection);
  }
}

} // namespace Nuki
