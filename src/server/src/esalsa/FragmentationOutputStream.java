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

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * @author Jason Maassen <J.Maassen@esciencecenter.nl>
 * @version 1.0
 * @since 1.0
 *
 */
public class FragmentationOutputStream {
    
    public static final int HEADER_LENGTH = 32;
    public static final byte OPCODE_SERVER = 2;   
    public static final int SERVER_PID = 0xFFFFFFFF;
    
    private final boolean littleEndian;
    private final int fragmentSize;    
    private final int destinationPID;    
    private final String name;
    
    private int transmitSequence;    
    private int ackSequence;    
    
    public FragmentationOutputStream(int destinationPID, boolean littleEndian, int fragmentSize, String name) {
        this.destinationPID = destinationPID;
        this.littleEndian = littleEndian;
        this.fragmentSize = fragmentSize;
        this.name = name;
    }

    private void writeIntLittleEndian(byte [] buffer, int offset, int value) {
        buffer[offset+0] = (byte)(0xff & value);
        buffer[offset+1] = (byte)(0xff & (value >> 8));
        buffer[offset+2] = (byte)(0xff & (value >> 16));
        buffer[offset+3] = (byte)(0xff & (value >> 24));
    }
        
    private void writeIntBigEndian(byte [] buffer, int offset, int value) {
        buffer[offset+0] = (byte)(0xff & (value >> 24));
        buffer[offset+1] = (byte)(0xff & (value >> 16));
        buffer[offset+2] = (byte)(0xff & (value >> 8));
        buffer[offset+3] = (byte)(0xff & value);
    }
    
    private void writeInt(byte [] buffer, int offset, int value) { 
        if (littleEndian) { 
            writeIntLittleEndian(buffer, offset, value);
        } else { 
            writeIntBigEndian(buffer, offset, value);            
        }
    }
    
    private void writeHeader(byte [] message, int payload) { 
        
        message[0] = 'M';
        message[1] = 'g'; 
        message[2] = 0;
        message[3] = (byte) OPCODE_SERVER ; 
        
        writeInt(message, 4, SERVER_PID);
        writeInt(message, 8, destinationPID);
        writeInt(message, 12, transmitSequence++);
        writeInt(message, 16, ackSequence);
        writeInt(message, 20, payload + HEADER_LENGTH);        
    }
    
    public byte [][] fragmentMessage(ServerMessage sm) throws IOException {
        
        EndianDataOutputStream out;
        
        ByteArrayOutputStream bao = new ByteArrayOutputStream();
        
        if (littleEndian) { 
            out = new LittleEndianDataOutputStream(bao);
        } else { 
            out = new BigEndianDataOutputStream(bao);
        }
        
        sm.write(out);
        
        byte [] buffer = bao.toByteArray();
        
        Logging.println("Fragmenting server message opcode=" + sm.opcode + " length=" + sm.length + " bytes=" + buffer.length);
                
        int position = 0;
        
        int messages = buffer.length / (fragmentSize-HEADER_LENGTH);
        
        if (buffer.length % (fragmentSize-HEADER_LENGTH) > 0) { 
            messages++;
        }
        
        byte [][] result = new byte[messages][];
        
        for (int i=0;i<messages;i++) {
            
            int payload = Math.min(buffer.length-position, fragmentSize-HEADER_LENGTH);
            
            byte [] message = new byte[HEADER_LENGTH + payload];
        
            Logging.println("  Fragment " + i + " bytes=" + message.length);
            
            writeHeader(message, payload);
            
            System.arraycopy(buffer, position, message, HEADER_LENGTH, payload);
            
            position += payload;
        
            result[i] = message;
        }
        
        return result;
    }
}
