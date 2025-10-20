#!/usr/bin/env python3
import socket
import time
import sys

def test_timeout(host='localhost', port=8080, timeout_test_duration=70):
    """
    Test server timeout by connecting but not sending data
    """
    print(f"Testing timeout on {host}:{port}")
    print(f"Will wait {timeout_test_duration} seconds to test timeout...")
    
    try:
        # Create socket and connect
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        print("✅ Connected to server")
        
        # Wait without sending anything
        print("⏱️  Waiting for server timeout...")
        start_time = time.time()
        
        # Try to receive data (should get 408 response when timeout occurs)
        sock.settimeout(timeout_test_duration + 10)  # Wait a bit longer than server timeout
        
        try:
            response = sock.recv(4096)
            elapsed = time.time() - start_time
            
            if response:
                print(f"✅ Received timeout response after {elapsed:.1f} seconds:")
                print(response.decode('utf-8', errors='ignore'))
                
                # Check if it's a 408 response
                if b'408' in response and b'Request Timeout' in response:
                    print("✅ SUCCESS: Server sent proper 408 timeout response!")
                    return True
                else:
                    print("❌ FAIL: Response is not a 408 timeout")
                    return False
            else:
                print("❌ FAIL: Connection closed without response")
                return False
                
        except socket.timeout:
            elapsed = time.time() - start_time
            print(f"❌ FAIL: No response received after {elapsed:.1f} seconds")
            return False
            
    except Exception as e:
        print(f"❌ ERROR: {e}")
        return False
    finally:
        sock.close()

def test_slow_request(host='localhost', port=8080):
    """
    Test server with slow request (should NOT timeout)
    """
    print(f"\nTesting slow request on {host}:{port}")
    
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        print("✅ Connected to server")
        
        # Send request very slowly (but within timeout)
        request_parts = [
            b"GET / HTTP/1.0\r\n",
            b"Host: localhost\r\n",
            b"\r\n"
        ]
        
        for part in request_parts:
            sock.send(part)
            print(f"📤 Sent: {part}")
            time.sleep(5)  # 5 second delay between parts
            
        # Should receive normal response
        sock.settimeout(10)
        response = sock.recv(4096)
        
        if response and b'200 OK' in response:
            print("✅ SUCCESS: Slow request handled correctly")
            return True
        else:
            print("❌ FAIL: Did not receive proper response")
            return False
            
    except Exception as e:
        print(f"❌ ERROR: {e}")
        return False
    finally:
        sock.close()

if __name__ == "__main__":
    host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8080
    
    print("🧪 WEBSERVER TIMEOUT TESTS")
    print("=" * 40)
    
    # Test 1: Connection timeout
    success1 = test_timeout(host, port)
    
    # Test 2: Slow but valid request
    success2 = test_slow_request(host, port)
    
    print("\n" + "=" * 40)
    print("📊 RESULTS:")
    print(f"Timeout test: {'✅ PASS' if success1 else '❌ FAIL'}")
    print(f"Slow request test: {'✅ PASS' if success2 else '❌ FAIL'}")
    
    if success1 and success2:
        print("🎉 ALL TESTS PASSED!")
        sys.exit(0)
    else:
        print("💥 SOME TESTS FAILED!")
        sys.exit(1)