package esalsa;

import java.io.IOException;
import java.io.InputStream;

public class NoisyInputStream extends InputStream {

	private final InputStream src;
	
	public NoisyInputStream(InputStream src) {
		this.src = src;
	}
	
	@Override
	public int read() throws IOException {

		int tmp = src.read();
		
		System.out.println("IN: " + (0xff & tmp) + " " + tmp);
		
		return tmp;
	}

}
