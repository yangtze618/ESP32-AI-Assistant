import requests
import serial
import time

# 1. 填入你刚刚申请的 API Key
API_KEY = "这里填入你的api key"

def chat_with_ai(user_text):
    url = "https://open.bigmodel.cn/api/paas/v4/chat/completions"
    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "Content-Type": "application/json"
    }
    data = {
        "model": "glm-4-flash", # 这是免费模型
        "messages": [
            {"role": "system", "content": "你是一个桌面嵌入式小助手。回答要非常简短，不超过20个字。"},
            {"role": "user", "content": user_text}
        ]
    }
    
    try:
        response = requests.post(url, headers=headers, json=data)
        result = response.json()
        return result['choices'][0]['message']['content']
    except Exception as e:
        return f"出错了: {e}"

if __name__ == "__main__":
    print("AI助手已启动，输入 'exit' 退出。")
    
    # ==========================================
    # 🚨 核心修改区：把下面 'COM3' 换成你的真实端口号！
    # 比如你在设备管理器看到的是 COM4，就改成 'COM4'
    # ==========================================
    ser = None
    try:
        ser = serial.Serial('COM10', 115200, timeout=1)
        print(f">> 硬件连接成功！当前使用的端口是: {ser.port}")
    except Exception as e:
        print(f">> 硬件连接失败，请检查端口号是否正确。报错信息: {e}")

    while True:
        text = input("你: ")
        if text == "exit":
            break
        
        print("AI思考中...")
        reply = chat_with_ai(text)
        print(f"AI: {reply}")
        
        # 准备把这句话发给 ESP32 的屏幕
        data_to_send = f"{reply}\n"
        
        if ser and ser.is_open:
            try:
                ser.write(data_to_send.encode('utf-8'))
                print(">> 已发送给 ESP32 硬件！")
            except Exception as e:
                print(f">> 发送失败: {e}")
        else:
            print(f">> [未连接硬件] 模拟发送: {data_to_send.strip()}")
        
            