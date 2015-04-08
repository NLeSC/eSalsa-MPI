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
public class ByteArrayOutputStream extends OutputStream {

    private byte [] buffer;
    private int index;
    
    public ByteArrayOutputStream(int size) {
        buffer = new byte[size];
    }

    @Override
    public void write(int b) throws IOException {
        buffer[index++] = (byte)(b & 0xFF);        
    }

    public void write(byte[] b) throws IOException {
        System.arraycopy(b, 0, buffer, index, b.length);
        index += b.length;
    }

    public void write(byte[] b, int off, int len) throws IOException {
        System.arraycopy(b, off, buffer, index, len);
        index += len;
    }

    public byte [] getBuffer() throws IOException {
        
        if (index != buffer.length) { 
            throw new IOException("Buffer not completely filled!");            
        }
        
        return buffer;
    }
}
