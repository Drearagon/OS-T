#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fstream>
#include <iostream>
#include <thread>
#include <SDL2/SDL.h>
#include <lvgl.h>
#include <lvgl/lv_drivers/sdl/sdl.h>
#include "cli/router.h"

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;

std::string processInput(const std::string& input) {
    return router::handleCommand(input);
}

void session(tcp::socket socket) {
    try {
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        if (websocket::is_upgrade(req)) {
            websocket::stream<tcp::socket> ws{std::move(socket)};
            ws.accept(req);
            for (;;) {
                boost::beast::flat_buffer buff;
                ws.read(buff);
                std::string cmd = boost::beast::buffers_to_string(buff.data());
                std::string result = processInput(cmd);
                ws.text(true);
                ws.write(boost::asio::buffer(result));
            }
        } else {
            std::ifstream file("src/ui/webcli/index.html");
            std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::content_type, "text/html");
            res.body() = body;
            res.prepare_payload();
            http::write(socket, res);
        }
    } catch (std::exception const& e) {
        std::cerr << "Session error: " << e.what() << std::endl;
    }
}

int main() {
    lv_init();
    SDL_Init(SDL_INIT_VIDEO);
    lv_display_t* disp = lv_sdl_window_create(480, 320);
    (void)disp;

    boost::asio::io_context ioc;
    tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};

    std::thread server([&]() {
        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread(session, std::move(socket)).detach();
        }
    });

    while (true) {
        lv_timer_handler();
        SDL_Delay(5);
    }

    server.join();
    SDL_Quit();
    return 0;
}
