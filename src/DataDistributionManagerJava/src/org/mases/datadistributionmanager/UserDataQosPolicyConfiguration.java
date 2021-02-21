/*
*  Copyright 2021 MASES s.r.l.
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  Refer to LICENSE for more information.
*/

package org.mases.datadistributionmanager;

import java.util.ArrayList;

public class UserDataQosPolicyConfiguration extends BaseConfiguration {
    /// <summary>
    /// Configuration key of <see cref="Period"/>
    /// </summary>
    public static final String ValuePropertyKey = "datadistributionmanager.opendds.qos.userdataqospolicy.value";

    /// <summary>
    /// Initialize a new instance of <see
    /// cref="UserDataQosPolicyConfiguration"/>
    /// </summary>
    public UserDataQosPolicyConfiguration() {
    }

    /**
     * See OpenDDS documentation
     * 
     * @return See OpenDDS documentation
     */
    public Byte[] getValue() {
        String value = keyValuePair.get(ValuePropertyKey);
        if (value == null)
            return null;
        if (value == "")
            return new Byte[0];
        ArrayList<Byte> lst = new ArrayList<Byte>();
        for (int i = 0; i < value.length() / 2; i++) {
            Byte val = Byte.valueOf(value.substring(i * 2, 2), 16);
            lst.add(val);
        }
        Byte[] bytes = new Byte[lst.size()];
        return lst.toArray(bytes);
    }

    /**
     * See OpenDDS documentation
     * 
     * @param value See OpenDDS documentation
     */
    public void setValue(Byte[] value) {
        if (value == null)
            keyValuePair.remove(ValuePropertyKey);
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < value.length; i++) {
            builder.append(String.format("%2x", value[i]));
        }
        keyValuePair.put(ValuePropertyKey, builder.toString());
    }
}
