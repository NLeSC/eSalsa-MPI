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

import java.io.ByteArrayInputStream;
import java.io.IOException;

/**
 * @author Jason Maassen <J.Maassen@esciencecenter.nl>
 * @version 1.0
 * @since 1.0
 *
 */
public class FragmentationInputStream {

    private static final int HEADER_LENGTH = FragmentationOutputStream.HEADER_LENGTH;
    private static final int SERVER_HEADER_LENGTH = HEADER_LENGTH + 4;
    
    private final String name;
    
    private final boolean littleEndian;
    
    private boolean firstInMessage = true;
    
    private int messageLength;
    
    private byte [] buffer;
    
    private int position;
    
    public FragmentationInputStream(boolean littleEndian, String name) {
        this.littleEndian = littleEndian;
        this.name = name;
    }

    private int readIntBigEndian(byte [] buffer, int offset) {
        return (((buffer[offset] & 0xff) << 24) | 
                ((buffer[offset+1] & 0xff) << 16) |
                ((buffer[offset+2] & 0xff) << 8)  | 
                 (buffer[offset+3] & 0xff));        
    }
    
    private int readIntLittleEndian(byte [] buffer, int offset) {
        return (((buffer[offset+3] & 0xff) << 24) | 
                ((buffer[offset+2] & 0xff) << 16) |
                ((buffer[offset+1] & 0xff) << 8)  | 
                 (buffer[offset] & 0xff));    
    }
    
    private int readInt(byte [] buffer, int offset) { 
     
        if (littleEndian) { 
            return readIntLittleEndian(buffer, offset);
        } else { 
            return readIntBigEndian(buffer, offset);
        }
    }
    
    private CommunicatorRequest decodeServerMessage() throws IOException {
        
        EndianDataInputStream in;
        
        if (littleEndian) { 
            in = new LittleEndianDataInputStream(new ByteArrayInputStream(buffer));
        } else { 
            in = new BigEndianDataInputStream(new ByteArrayInputStream(buffer));
        }

        int opcode = in.readInt();
        
        CommunicatorRequest req = null;

        Logging.println("VirtualConnection " + name + " received opcode " + opcode);

        switch (opcode) {

        case Protocol.OPCODE_SPLIT:
            Logging.println("VirtualConnection " + name + " - Reading COMM message.");
            req = new SplitRequest(in);
            break;

        case Protocol.OPCODE_GROUP:
            Logging.println("VirtualConnection " + name +" - Reading GROUP message.");
            req = new GroupRequest(in);
            break;

        case Protocol.OPCODE_DUP:
            Logging.println("VirtualConnection " + name + " - Reading DUP message.");
            req = new DupRequest(in);
            break;

        case Protocol.OPCODE_FREE:
            Logging.println("VirtualConnection " + name + " - Reading FREE message.");
            req = new FreeRequest(in);
            break;

        case Protocol.OPCODE_FINALIZE:
            Logging.println("VirtualConnection " + name + " - Reading FINALIZE message.");
            req = new FinalizeRequest(in);
            Logging.println("VirtualConnection " + name + " - Delivering FINALIZE message.");
            break;
        case Protocol.OPCODE_CLOSE_LINK:
            Logging.println("VirtualConnection " + name + " - Closing link.");
            req = null;
            break;

        default:
            Logging.println("VirtualConnection " + name + " sent illegal opcode " + opcode);
            throw new IOException("Illegal opcode " + opcode + " sent by cluster " + name);
        }
        
        return req;
        
    }
    
    public CommunicatorRequest addMessage(byte [] message, int length) throws IOException { 
        
        if (firstInMessage) {             
            // This is the first server message in a sequence.
            if (length < SERVER_HEADER_LENGTH) { 
                throw new IOException("ServerMessage is too small!");
            }

            messageLength = readInt(message, HEADER_LENGTH + 4);
            buffer = new byte[messageLength];
            position = 0;
            firstInMessage = false;
        }
        
        Logging.println("Copying message " + length + " " + messageLength  + " " + position + " " + HEADER_LENGTH);
        
        System.arraycopy(message, HEADER_LENGTH, buffer, position, length-HEADER_LENGTH);
        
        position += length-HEADER_LENGTH;
        
        if (position == messageLength) { 
            // We've have defragmented a complete server message, so decode it!
            CommunicatorRequest req = decodeServerMessage();            
            firstInMessage = true;
            message = null;
            position = 0;
            return req;
        }
        
        return null;
    }
}
