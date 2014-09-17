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

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;

/**
 * @author Jason Maassen <J.Maassen@esciencecenter.nl>
 * @version 1.0
 * @since 1.0
 *
 */
public abstract class EndianDataInputStream {
    
    protected final InputStream in;
    
    public EndianDataInputStream(InputStream in) {
        this.in = in;
    }

    public boolean readBoolean() throws IOException {
        return (readByte() != 0);
    }

    public byte readByte() throws IOException {
        return (byte) in.read();
    }

    public double readDouble() throws IOException {
        return Double.longBitsToDouble(readLong());
    }
    
    public float readFloat() throws IOException {
        return Float.intBitsToFloat(readInt());
    }

    public void readFully(byte[] b) throws IOException {
        readFully(b, 0, b.length);
    }
   
    public void readFully(byte[] b, int off, int len) throws IOException {
        
        int pos = 0; 
        int read;
        
        while (pos != len) {
            read = in.read(b, pos + off, len-pos);
        
            if (read < 0) { 
                throw new EOFException();
            }
            
            pos += read;
        }
    }
   
    public int readUnsignedByte() throws IOException {
        return in.read();
    }

    public int skipBytes(int n) throws IOException {
        return (int) in.skip(n);
    }
     
    public void close() throws IOException {
        in.close();
    }
    
    public abstract char readChar() throws IOException;
    public abstract int readInt() throws IOException;
    public abstract long readLong() throws IOException;
    public abstract short readShort() throws IOException;   
    public abstract int readUnsignedShort() throws IOException;
}
