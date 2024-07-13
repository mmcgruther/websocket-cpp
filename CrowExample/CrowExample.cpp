// CrowExample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "crow.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

// Function to generate random data
std::vector<int> generateRandomData(int size) {
   std::vector<int> data(size);
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<> dis(1, 100);
   for (auto& value : data) {
      value = dis(gen);
   }
   return data;
}

int main() {
   std::thread crowThread([&]() {
      crow::SimpleApp crowapp;

   CROW_ROUTE(crowapp, "/hello")([]() {
      return "Hello from C++!";
      });

   crowapp.port(18080).multithreaded().run();
      });


   auto* app = new uWS::App();

   // Create a thread to send data periodically
   std::thread dataThread([&]() {
      while (true) {
         std::this_thread::sleep_for(std::chrono::seconds(1));
         std::vector<int> data = generateRandomData(10);
         std::string dataString;
         for (const auto& value : data) {
            dataString += std::to_string(value) + ",";
         }
         dataString.pop_back(); // Remove trailing comma

         // Broadcast data to all connected clients
         app->getLoop()->defer([=]() {app->publish("topic", dataString, uWS::OpCode::TEXT); });
         
      }
      });

   struct PerSocketData {

   };
   app->ws<PerSocketData>("/*", {
       .open = [](auto* ws) {
           std::cout << "A client connected!" << std::endl;
           ws->subscribe("topic");
       },
       .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
           std::cout << "Received message: " << message << std::endl;
       },
       .close = [](auto* ws, int code, std::string_view message) {
           std::cout << "A client disconnected!" << std::endl;
       }
      }).get("/*", [](auto* res, auto*/*req*/) {
       std::cout << "Hello world!" << std::endl;
       res->end("Hello world!");
         }).listen(9001, [](auto* token) {
            if (token) {
               std::cout << "Listening on port 9001" << std::endl;
            }
            }).run();
            std::cout << "Failed to listen on port 9001" << std::endl;
            dataThread.join();

            return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
