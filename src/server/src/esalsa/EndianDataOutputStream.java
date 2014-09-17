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
public abstract class EndianDataOutputStream {

    protected final OutputStream out;
    
    public EndianDataOutputStream(OutputStream out) {
        this.out = out;
    }

    public void write(int value) throws IOException {
        out.write(value);
    }

    public void write(byte[] b) throws IOException {
        out.write(b);
    }

    public void write(byte[] b, int off, int len) throws IOException {
        out.write(b, off, len);
    }

    public void writeBoolean(boolean value) throws IOException {
        out.write(value ? 1 : 0);
    }

    public void writeByte(int value) throws IOException {
        out.write(value);
    }

    public void writeBytes(String s) throws IOException {
        out.write(s.getBytes());
    }

    public void writeDouble(double value) throws IOException {
        writeLong(Double.doubleToLongBits(value));
    }

    public void writeFloat(float value) throws IOException {
        writeInt(Float.floatToIntBits(value));
    }

    public void flush() throws IOException { 
        out.flush();
    }
    
    public void close() throws IOException {
        out.close();
    }
    
    public abstract void writeChar(int v) throws IOException;

    public abstract void writeInt(int v) throws IOException;
    
    public abstract void writeLong(long v) throws IOException;
    
    public abstract void writeShort(int v) throws IOException;    
    
}
