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
import java.io.InputStream;

/**
 * @author Jason Maassen <J.Maassen@esciencecenter.nl>
 * @version 1.0
 * @since 1.0
 *
 */
public class BigEndianDataInputStream extends EndianDataInputStream {

    private final byte [] buffer = new byte[8];
    
    public BigEndianDataInputStream(InputStream in) {
        super(in);
    }

    @Override
    public char readChar() throws IOException {
        readFully(buffer, 0, 2);
        return (char)((buffer[0] << 8) | (buffer[1] & 0xff));
    }
    
    @Override
    public int readInt() throws IOException {
        readFully(buffer, 0, 4);
        return (((buffer[0] & 0xff) << 24) | 
                ((buffer[1] & 0xff) << 16) |
                ((buffer[2] & 0xff) << 8)  | 
                 (buffer[3] & 0xff));
    }

    @Override
    public long readLong() throws IOException {
        readFully(buffer, 0, 8);
        return (((long)(buffer[0] & 0xff) << 56) |
                ((long)(buffer[1] & 0xff) << 48) |
                ((long)(buffer[2] & 0xff) << 40) |
                ((long)(buffer[3] & 0xff) << 32) |
                ((long)(buffer[4] & 0xff) << 24) |
                ((long)(buffer[5] & 0xff) << 16) |
                ((long)(buffer[6] & 0xff) <<  8) |
                ((long)(buffer[7] & 0xff)));
    }

    @Override
    public short readShort() throws IOException {
        readFully(buffer, 0, 2);
        return (short)((buffer[0] << 8) | (buffer[1] & 0xff));
    }

    @Override
    public int readUnsignedShort() throws IOException {
        readFully(buffer, 0, 2);
        return (((buffer[0] & 0xff) << 8) | (buffer[1] & 0xff));
    }
}
