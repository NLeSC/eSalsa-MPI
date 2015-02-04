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

/**
 * @author Jason Maassen <J.Maassen@esciencecenter.nl>
 * @version 1.0
 * @since 1.0
 *
 */
public class CommunicatorRequest extends ServerMessage {

    /** Target communicator. */
    public final int communicator;
    
    /** Rank in target communicator. */
    public final int rank;

    /**
     * Constructor that reads a CommunicatorRequest from a {@link DataInputStream}.
     * 
     * @param opcode
     *          The request opcode (which has already been read).
     * @param in
     *          The {@link DataInputStream} from which the rest of the request must be read.
     * @throws IOException
     *          If an IO error occurred while reading the request. 
     */
    protected CommunicatorRequest(int opcode, EndianDataInputStream in) throws IOException {
        super(opcode, in);
        communicator = in.readInt();
        rank = in.readInt();
    }
}
