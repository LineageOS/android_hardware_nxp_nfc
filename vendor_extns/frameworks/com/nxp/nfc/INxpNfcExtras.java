/*
 * Copyright (C) 2025 The Android Open Source Project
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

package com.nxp.nfc;

import android.app.Activity;

import java.io.IOException;

import com.nxp.nfc.NxpNfcAdapter.NxpReaderCallback;

/**
 * @class INxpNfcExtras
 * @brief Interface to perform the OEM Extension functionality.
 *
 */
public interface INxpNfcExtras {

    /**
     * @brief Updates the RF configuration when cover state is changed
     *
     * @param attached
     * @param coverType
     *
     * @return void
     */
    void coverAttached(boolean attached, int coverType);

    /**
     * @brief starts the cover auth
     *
     * @return response
     */
    byte[] startCoverAuth();

    /**
     * @brief stops the cover auth
     *
     * @return true on success else false
     */
    boolean stopCoverAuth();

    /**
     * @brief trans the passed data
     *
     * @param data
     *
     * @return response
     */
    byte[] transceiveAuthData(byte[] data);

    /**
     * @brief starts the Prbs test
     *
     * @return response
     */
    byte[] prbsTest(int action, int tech, int speed);

    /**
     * @brief starts the getResoFreq
     *
     * @return response
     */
    byte[] getResoFreqTest();

    /**
     * gets the state of the Secure Element
     */
    byte[] getSwpStateTest(int activeSe);

    /**
     * @brief reset ESE
     *
     * @return true on success else false
     */
    boolean halResetEse();

    /**
     * @brief set eUICC Default Port
     * @param port
     *
     * @return None
     */
    void setEUICCDefaultPort(int port);

    /**
     * @brief get NCI version
     *
     * @return response
     */
    byte[] getNciVersion();

    /**
     * @brief Set T4T Contactless Reader Mode
     * @param enable true to turn on T4T mode false to turn off
     * @return byte array with status code.
     *         byte[0] indicates status.
     *         0x00 indicates success
     *         0x03 indicates failure
     */
    byte[] setT4TMode(boolean enable, Activity activity,
                      NxpReaderCallback t4tTagCallback) throws IOException;
}
