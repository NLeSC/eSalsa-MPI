/*
 * Copyright 2013 Netherlands eScience Center
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

package esalsa;

import java.io.IOException;
import java.io.OutputStream;

/**
 * @author Jason Maassen <J.Maassen@esciencecenter.nl>
 * @version 1.0
 * @since 1.0
 *
 */
public class LittleEndianDataOutputStream extends EndianDataOutputStream {

    private final byte [] buffer = new byte[8];
    
    public LittleEndianDataOutputStream(OutputStream out) {
        super(out);
    }

    @Override
    public void writeChar(int v) throws IOException {
        buffer[0] = (byte)(0xff & v);
        buffer[1] = (byte)(0xff & (v >> 8));
        out.write(buffer, 0, 2);
    }

    @Override
    public void writeShort(int v) throws IOException {
        buffer[0] = (byte)(0xff & v);
        buffer[1] = (byte)(0xff & (v >> 8));
        out.write(buffer, 0, 2);        
    }
    
    @Override
    public void writeInt(int v) throws IOException {
        buffer[0] = (byte)(0xff & v);
        buffer[1] = (byte)(0xff & (v >> 8));
        buffer[2] = (byte)(0xff & (v >> 16));
        buffer[3] = (byte)(0xff & (v >> 24));
        out.write(buffer, 0, 4);
    }

    @Override
    public void writeLong(long v) throws IOException {
        buffer[0] = (byte)(0xff & v);
        buffer[1] = (byte)(0xff & (v >>  8));
        buffer[2] = (byte)(0xff & (v >> 16));
        buffer[3] = (byte)(0xff & (v >> 24));
        buffer[4] = (byte)(0xff & (v >> 32));
        buffer[5] = (byte)(0xff & (v >> 40));
        buffer[6] = (byte)(0xff & (v >> 48));
        buffer[7] = (byte)(0xff & (v >> 56));
        out.write(buffer, 0, 8);
    }
}
