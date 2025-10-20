#!/usr/bin/env python3
import socket
import threading
import time
import sys

def create_idle_connection(host, port, connection_id, duration=70):
    """Create a connection that stays idle"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        print(f"Connection {connection_id}: Connected")
        
        # Wait for timeout
        start_time = time.time()
        sock.settimeout(duration + 10)
        
        try:
            response = sock.recv(4096)
            elapsed = time.time() - start_time
            if b'408' in response:
                print(f"Connection {connection_id}: ✅ Timed out after {elapsed:.1f}s")
            else:
                print(f"Connection {connection_id}: ❌ Unexpected response")
        except socket.timeout:
            print(f"Connection {connection_id}: ❌ No timeout response received")
            
    except Exception as e:
        print(f"Connection {connection_id}: ❌ Error: {e}")
    finally:
        sock.close()

def test_multiple_timeouts(host='localhost', port=8080, num_connections=5):
    """Test multiple simultaneous timeout connections"""
    print(f"🧪 Testing {num_connections} simultaneous timeout connections")
    
    threads = []
    for i in range(num_connections):
        thread = threading.Thread(
            target=create_idle_connection,
            args=(host, port, i+1)
        )
        threads.append(thread)
        thread.start()
        time.sleep(1)  # Small delay between connections
    
    # Wait for all threads
    for thread in threads:
        thread.join()
    
    print("✅ All timeout tests completed")

if __name__ == "__main__":
    host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8080
    num = int(sys.argv[3]) if len(sys.argv) > 3 else 5
    
    test_multiple_timeouts(host, port, num)
