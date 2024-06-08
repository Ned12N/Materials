import time 
start_time = time.time()
import subprocess


ret = subprocess.run("verify_rsa_pss.exe",text=True,capture_output=True, encoding='utf-8')
end_time = time.time()

total_time = (end_time - start_time)*1000
print(total_time)
print(ret.stdout)