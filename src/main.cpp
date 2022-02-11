#include <parallel_hashmap/phmap.h>
#include <unidokkan/database.h>
#include <unidokkan/log.h>
#include <unidokkan/hook.h>
#include <unidokkan/errors.h>
#include <dokkan/crypto.h>
#include <nlohmann/json.hpp>
#include <dokkan/defs.h>
#include <restclient-cpp/restclient.h>
#include <fmt/format.h>
#include <dokkan/files.h>
#include <iostream>
#include <fstream>
#include <dokkan/instances.h>
#include <cocos/ui/UIWidget.h>
#include <cocos/ui/UIButton.h>
#include <unidokkan/ui/scene.h>
#include <unidokkan/ui/layout.h>
#include <unidokkan/ui/button.h>
#include <unidokkan/ui/imageview.h>
using namespace std::string_view_literals;
using json = nlohmann::json;
using namespace phmap;
using namespace UniDokkan::UI;
namespace cocos2d = ud_cocos2d;

HookLibV4* kHookLib;
std::string string_to_hex(const std::string& input){
    static const char hex_digits[] = "0123456789ABCDEF";
    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input){
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}
int hex_value(char hex_digit){
    switch (hex_digit) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return hex_digit - '0';

    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return hex_digit - 'A' + 10;

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return hex_digit - 'a' + 10;
    }
    throw std::invalid_argument("invalid hex digit");
}
std::string hex_to_string(const std::string& input){
    const auto len = input.length();
    if (len & 1) throw std::invalid_argument("odd length");

    std::string output;
    output.reserve(len / 2);
    for (auto it = input.begin(); it != input.end(); )
    {
        int hi = hex_value(*it++);
        int lo = hex_value(*it++);
        output.push_back(hi << 4 | lo);
    }
    return output;
}
bool InRange(int tocheck, int min, int max) {
	// UD_LOGI("In range min = %d",min);
	// UD_LOGI("In range max = %d",max);
	// UD_LOGI("In range Tocheck = %d",tocheck);
	if (tocheck == min) {
		// UD_LOGI("In range Tocheck is Min (true)");
		return true;
	} else {
		if (tocheck == max) {
			// UD_LOGI("In range Tocheck is Max (true)");
			return true;
		} else {
			if (tocheck < max) {
				if (tocheck > min) {
					// UD_LOGI("In range Tocheck is highter than min and lower than max (true)");
					return true;
				} else {
					// UD_LOGI("In range Tocheck is lower min and lower than max (false)");
					return false;
				}
			} else {
				// UD_LOGI("In range Tocheck is bigger than max (false)");
				return false;
			}
		}
	}
	return false;
	// UD_LOGI("In range should not be there!");
}
void DownloadFile(std::string url, std::string destination){
	destination = "/storage/sdcard0/Pictures/" + destination;
	UD_LOGI("Downloading file to %s",destination.c_str());
	auto Downloaded_file_raw = RestClient::get(url);
	std::ofstream output_file(destination, std::ios::binary | std::ios::out);
	if (output_file.is_open()) {
		output_file << Downloaded_file_raw.body;
		output_file.close();
	} else {
		UD_LOGI("Could not download file to %s",destination.c_str());
	}
}
bool ShowEventsBanners(NetworkResponse *response) {
	UD_LOGI("DumpEventsIMG : Loading . . .");
	std::string banner_l_img_path	= "DokkanEvent/image/quest_list_banner_";
	std::string banner_t_img_path	= "DokkanEvent/image/quest_top_banner_";
	std::string banner_e_img_path	= "DokkanEvent/image/quest_event_banner_";
	std::string banner_eza_img_path	= "DokkanEvent/image/zbattle_list_banner_";
	auto Check_eventkagi_events		=	response->jsonBody.find("eventkagi_events");
	if (Check_eventkagi_events != response->jsonBody.end()) {
		for (auto &EventList_Kagi : response->jsonBody["eventkagi_events"]) {
			std::string Event_ID_long 		= 	to_string(EventList_Kagi["id"]);
			std::string Event_ID 			= 	Event_ID_long.substr(0, 3);
			int Event_ID_int 				= 	std::stoi(Event_ID);
			auto Check_banner_image		=	EventList_Kagi.find("banner_image");
			if (Check_banner_image != EventList_Kagi.end()) {
				if (EventList_Kagi["banner_image"].is_null() == true) { continue; }
				std::string banner_image = EventList_Kagi["banner_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",banner_image.c_str(),banner_l_img_path.c_str(),Event_ID_int);
				DownloadFile(banner_image,banner_l_img_path+std::to_string(Event_ID_int)+".png");
			}
			auto Check_event_image		=	EventList_Kagi.find("event_image");
			if (Check_event_image != EventList_Kagi.end()) {
				if (EventList_Kagi["event_image"].is_null() == true) { continue; }
				std::string event_image = EventList_Kagi["event_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",event_image.c_str(),banner_t_img_path.c_str(),Event_ID_int);
				DownloadFile(event_image,banner_t_img_path+std::to_string(Event_ID_int)+".png");
			}
			auto Check_minibanner_image		=	EventList_Kagi.find("minibanner_image");
			if (Check_minibanner_image != EventList_Kagi.end()) {
				if (EventList_Kagi["minibanner_image"].is_null() == true) { continue; }		
				std::string minibanner_image = EventList_Kagi["minibanner_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",minibanner_image.c_str(),banner_e_img_path.c_str(),Event_ID_int);
				DownloadFile(minibanner_image,banner_e_img_path+std::to_string(Event_ID_int)+".png");
			}
		}
	}
	
	auto Check_eventkagi_eventkagi_z_battle_stages		=	response->jsonBody.find("eventkagi_z_battle_stages");
	if (Check_eventkagi_eventkagi_z_battle_stages != response->jsonBody.end()) {
		for (auto &EZAList_Kagi : response->jsonBody["eventkagi_z_battle_stages"]) {
			std::string Event_ID_long 		= 	to_string(EZAList_Kagi["id"]);
			std::string Event_ID 			= 	Event_ID_long.substr(0, 3);
			int Event_ID_int 				= 	std::stoi(Event_ID);
			auto Check_banner_image		=	EZAList_Kagi.find("banner_image");
			if (Check_banner_image != EZAList_Kagi.end()) {
				if (EZAList_Kagi["banner_image"].is_null() == true) { continue; }		
				std::string banner_image = EZAList_Kagi["banner_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",banner_image.c_str(),banner_eza_img_path.c_str(),Event_ID_int);
				DownloadFile(banner_image,banner_eza_img_path+std::to_string(Event_ID_int)+".png");
			}
		}
	}

	auto Check_events		=	response->jsonBody.find("events");
	if (Check_events != response->jsonBody.end()) {
		for (auto &EventList : response->jsonBody["events"]) {
			std::string Event_ID_long 		= 	to_string(EventList["id"]);
			std::string Event_ID 			= 	Event_ID_long.substr(0, 3);
			int Event_ID_int 				= 	std::stoi(Event_ID);
			auto Check_banner_image		=	EventList.find("banner_image");
			if (Check_banner_image != EventList.end()) {
				if (EventList["banner_image"].is_null() == true) { continue; }
				std::string banner_image = EventList["banner_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",banner_image.c_str(),banner_l_img_path.c_str(),Event_ID_int);
				DownloadFile(banner_image,banner_l_img_path+std::to_string(Event_ID_int)+".png");
			}
			auto Check_event_image		=	EventList.find("event_image");
			if (Check_event_image != EventList.end()) {
				if (EventList["event_image"].is_null() == true) { continue; }
				std::string event_image = EventList["event_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",event_image.c_str(),banner_t_img_path.c_str(),Event_ID_int);
				DownloadFile(event_image,banner_t_img_path+std::to_string(Event_ID_int)+".png");
			}
			auto Check_minibanner_image		=	EventList.find("minibanner_image");
			if (Check_minibanner_image != EventList.end()) {	
				if (EventList["minibanner_image"].is_null() == true) { continue; }			
				std::string minibanner_image = EventList["minibanner_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",minibanner_image.c_str(),banner_e_img_path.c_str(),Event_ID_int);
				DownloadFile(minibanner_image,banner_e_img_path+std::to_string(Event_ID_int)+".png");
			}
		}
	}
	
	auto Check_z_battle_stages		=	response->jsonBody.find("z_battle_stages");
	if (Check_z_battle_stages != response->jsonBody.end()) {
		for (auto &EZAList : response->jsonBody["z_battle_stages"]) {
			std::string Event_ID_long 		= 	to_string(EZAList["id"]);
			std::string Event_ID 			= 	Event_ID_long.substr(0, 3);
			int Event_ID_int 				= 	std::stoi(Event_ID);
			auto Check_banner_image		=	EZAList.find("banner_image");
			if (Check_banner_image != EZAList.end()) {
				if (EZAList["banner_image"].is_null() == true) { continue; }
				std::string banner_image = EZAList["banner_image"].get<std::string>();
				// UD_LOGI("Save : %s \nas %s%d.png",banner_image.c_str(),banner_eza_img_path.c_str(),Event_ID_int);
				DownloadFile(banner_image,banner_eza_img_path+std::to_string(Event_ID_int)+".png");
			}
		}
	}
	
    return true;
}
extern "C" {
    __attribute__ ((visibility ("default"))) 
    int unidokkan_init_v4(HookLibV4 *hook_lib) {
        UD_LOGI("DumpEventsIMG custom module loading...");

        if (hook_lib->size < sizeof(HookLibV4)) {
            return UD_MODULE_INVALID_SIZE;
        }

        if (hook_lib->version < kMinPatcherVer) {
            return UD_MODULE_INVALID_VERSION;
        }
		hook_lib->addResponseHook("^/events$", ShowEventsBanners);
        UD_LOGI("DumpEventsIMG module successfully loaded.");
        return UD_MODULE_SUCCESS;
    }
}