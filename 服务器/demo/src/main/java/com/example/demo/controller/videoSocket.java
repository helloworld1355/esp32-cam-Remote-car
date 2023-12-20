package com.example.demo.controller;

import jakarta.websocket.OnClose;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;
import org.springframework.stereotype.Component;


import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ConcurrentHashMap;

@Component
@ServerEndpoint("/")
public class videoSocket {

    public videoSocket(){
        Thread t = new Thread(() -> {
            System.out.println("进入子线程");
            if(!isListen){
                isListen = true;
                sockListen();
            }
        });
        t.start();
    }

    private static boolean isListen = false;
    private static Socket clientSocket;
    private String id = "";
    private Session session;

    private final int port = 81;
    private static int onlineCount = 0;
    /**
     * 用来存储当前在线的客户端(此map线程安全)
     */
    private static ConcurrentHashMap<String, videoSocket> webSocketMap = new ConcurrentHashMap<>();

    public void sendMessage(String str) throws IOException {
        session.getBasicRemote().sendText(str);
    }

    public void sendToAll(String message) throws IOException {
        for (String key : webSocketMap.keySet()) {
            webSocketMap.get(key).sendMessage(message);
        }
    }
/**
 * @author qiusuyang
 * @date   2020/10/25 14:40。
 * @description 监听80端口，这个端口是esp32连接所用。本函数调用了websocket，用来转发图片数据给连接到本服务的客户端。
 * 用于esp32上传给服务器端图片流，图片流采用base64编码传输，暂时未试过直接传输图片数据，
 * 因为分片麻烦，还未成功实现。故直接将数据发送过来
*/
    private void sockListen() {

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            System.out.println("Server is listening on port " + port);
            int i = 0;
            while (true) {
                i++;
                // 等待客户端连接
                System.out.println("i = "+i);
                clientSocket = serverSocket.accept();
                clientSocket.setSoTimeout(8000);
                while(!clientSocket.isClosed()){
                    System.out.println("connect!!");
                    try (BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream(), StandardCharsets.UTF_8))) {
                        // 读取图像数据
                        String result ;
                        while ((result = reader.readLine()) != null) {
                            sendToAll(result);
                        }

                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                // 关闭客户端连接
                clientSocket.close();
                System.out.println("Client disconnected");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
//        });
    }

/**
 * @author qiusuyang
 * @param message 从前端发送往esp32的控制信号
 * @description 接收从websocket接收到的信号，转发给esp32
*/
    public void sendToClient(String message){
        System.out.println("进入发送消息");
        if (clientSocket == null){
            return ;
        }
        try{
            OutputStream outputStream = clientSocket.getOutputStream();
            outputStream.write(message.getBytes());
            System.out.println("发送！！！");
        }catch (IOException e){
            System.out.println(e.getMessage());
            System.out.println("Error sending message to client");
        }
    }



    @OnOpen
    public void onOpen(Session session) {
        onlineCount++;
        this.session = session;
        this.id = session.getId();
        webSocketMap.put(this.id, this);
        System.out.println("WebSocket Connection Opened: " + session.getId());
    }

    @OnMessage
    public void onMessage(String message, Session session) {
        System.out.println("发送！！！");
        sendToClient(message);
    }

    @OnClose
    public void onClose(Session session) {
        System.out.println("WebSocket Connection Closed: " + session.getId());
    }



}
