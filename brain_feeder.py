import socket
import time
import sys

# Port 8109 is the Background Svadhyaya Port in v7.0
HOST = '127.0.0.1'
PORT = 8109 

def feed_kernel(text_data):
    try:
        # Create a raw TCP socket connection
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        
        print(f"[+] Sending payload ({len(text_data)} bytes)...")
        # Send raw string data to the C-kernel
        s.sendall(text_data.encode('utf-8'))
        
        # Give kernel a fraction of a second to Holographically Fold it
        time.sleep(0.1) 
        s.close()
        print("[✓] Successfully absorbed by Subconscious ROM.\n")
    except ConnectionRefusedError:
        print("[!] ERROR: Param-Tattva v7.0 is not running. Start the C-Kernel first!")

if __name__ == "__main__":
    print("========================================")
    print("   PARAM-TATTVA GURUKUL FEEDER STREAM   ")
    print("========================================")
    
    # You can loop through a massive text file here. For demo, we take inputs.
    print("Type a concept to silently feed to the OS (Type 'exit' to quit):")
    
    while True:
        data = input(">> ")
        if data.lower() == 'exit':
            break
        if data.strip():
            feed_kernel(data)

