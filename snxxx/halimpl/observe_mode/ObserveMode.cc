/*
 * Copyright 2024-2025 NXP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <ObserveMode.h>
#include <phNfcNciConstants.h>

#include <vector>

#include "NciDiscoveryCommandBuilder.h"
#include "phNxpNciHal_extOperations.h"

using namespace std;

bool bIsObserveModeEnabled;

/*******************************************************************************
 *
 * Function         setObserveModeFlag()
 *
 * Description      It sets the observe mode flag
 *
 * Parameters       bool - true to enable observe mode
 *                         false to disable observe mode
 *
 * Returns          void
 *
 ******************************************************************************/
void setObserveModeFlag(bool flag) { bIsObserveModeEnabled = flag; }

/*******************************************************************************
 *
 * Function         isObserveModeEnabled()
 *
 * Description      It gets the observe mode flag
 *
 * Returns          bool true if the observed mode is enabled
 *                  otherwise false
 *
 ******************************************************************************/
bool isObserveModeEnabled() { return bIsObserveModeEnabled; }

/*******************************************************************************
 *
 * Function         handleObserveMode()
 *
 * Description      This handles the ObserveMode command and enables the observe
 *                  Mode flag
 *
 * Returns          It returns number of bytes received.
 *
 ******************************************************************************/
int handleObserveMode(uint16_t data_len, const uint8_t* p_data) {
  if (data_len <= 4) {
    return 0;
  }

  uint8_t status = NCI_RSP_FAIL;
  if (phNxpNciHal_isObserveModeSupported()) {
    setObserveModeFlag(p_data[NCI_MSG_INDEX_FEATURE_VALUE]);
    status = NCI_RSP_OK;
  }

  phNxpNciHal_vendorSpecificCallback(
      p_data[NCI_OID_INDEX], p_data[NCI_MSG_INDEX_FOR_FEATURE], {status});

  return p_data[NCI_MSG_LEN_INDEX];
}

/*******************************************************************************
 *
 * Function         handleObserveModeTechCommand()
 *
 * Description      This handles the ObserveMode command and enables the observe
 *                  Mode flag
 *
 * Returns          It returns number of bytes received.
 *
 ******************************************************************************/
int handleObserveModeTechCommand(uint16_t data_len, const uint8_t* p_data) {
  uint8_t status = NCI_RSP_FAIL;
  if (phNxpNciHal_isObserveModeSupported() &&
      (p_data[NCI_MSG_INDEX_FEATURE_VALUE] ==
           OBSERVE_MODE_TECH_COMMAND_SUPPORT_FLAG ||
       p_data[NCI_MSG_INDEX_FEATURE_VALUE] ==
           OBSERVE_MODE_TECH_COMMAND_SUPPORT_FLAG_FOR_ALL_TECH ||
       p_data[NCI_MSG_INDEX_FEATURE_VALUE] ==
           NCI_ANDROID_PASSIVE_OBSERVE_PARAM_DISABLE)) {
    bool flag = (p_data[NCI_MSG_INDEX_FEATURE_VALUE] ==
                     OBSERVE_MODE_TECH_COMMAND_SUPPORT_FLAG ||
                 p_data[NCI_MSG_INDEX_FEATURE_VALUE] ==
                     OBSERVE_MODE_TECH_COMMAND_SUPPORT_FLAG_FOR_ALL_TECH)
                    ? true
                    : false;
    uint8_t rf_deactivate_cmd[] = {0x21, 0x06, 0x01, 0x00};

    // send RF Deactivate command
    NFCSTATUS rfDeactivateStatus =
        phNxpNciHal_send_ext_cmd(sizeof(rf_deactivate_cmd), rf_deactivate_cmd);
    if (rfDeactivateStatus == NFCSTATUS_SUCCESS) {
      if (flag) {
        // send Observe Mode Tech command
        NFCSTATUS nciStatus =
            phNxpNciHal_send_ext_cmd(data_len, (uint8_t*)p_data);
        if (nciStatus != NFCSTATUS_SUCCESS) {
          NXPLOG_NCIHAL_E("%s ObserveMode tech command failed", __func__);
        }
      }

      // send Discovery command
      vector<uint8_t> discoveryCommand =
          flag ? NciDiscoveryCommandBuilderInstance.reConfigRFDiscCmd()
               : NciDiscoveryCommandBuilderInstance.getDiscoveryCommand();
      NFCSTATUS rfDiscoveryStatus = phNxpNciHal_send_ext_cmd(
          discoveryCommand.size(), &discoveryCommand[0]);

      if (rfDiscoveryStatus == NFCSTATUS_SUCCESS) {
        setObserveModeFlag(flag);
        status = NCI_RSP_OK;
      } else {
        NXPLOG_NCIHAL_E("%s Rf Disovery command failed", __func__);
      }

    } else {
      NXPLOG_NCIHAL_E("%s Rf Deactivate command failed", __func__);
    }
  } else {
    NXPLOG_NCIHAL_E(
        "%s ObserveMode feature or tech which is requested is not supported",
        __func__);
  }

  phNxpNciHal_vendorSpecificCallback(
      p_data[NCI_OID_INDEX], p_data[NCI_MSG_INDEX_FOR_FEATURE], {status});

  return p_data[NCI_MSG_LEN_INDEX];
}

/*******************************************************************************
 *
 * Function         handleGetObserveModeStatus()
 *
 * Description      Handles the Get Observe mode command and gives the observe
 *                  mode status
 *
 * Returns          It returns number of bytes received.
 *
 ******************************************************************************/
int handleGetObserveModeStatus(uint16_t data_len, const uint8_t* p_data) {
  // 2F 0C 01 04 => ObserveMode Status Command length is 4 Bytes
  if (data_len < 4) {
    return 0;
  }
  vector<uint8_t> response;
  response.push_back(NCI_RSP_OK);
  response.push_back(isObserveModeEnabled() ? 0x01 : 0x00);
  phNxpNciHal_vendorSpecificCallback(p_data[NCI_OID_INDEX],
                                     p_data[NCI_MSG_INDEX_FOR_FEATURE],
                                     std::move(response));

  return p_data[NCI_MSG_LEN_INDEX];
}
