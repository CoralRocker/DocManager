#include "gui.hpp"

// ImGui
#include "graph.hpp"
#include "imgui.h"

// C++ Includes
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

// using std::cout, std::endl;

/**
 * @brief Display a progress bar as we parse through the documents 
 *
 * @returns False until the documents are parsed.
 */
bool parseDocs(docgraph& graph) {

  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;

  // Set Next Window Size
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  
  static unsigned idx = 0;
  if( idx >= graph.size() ) return true;

  ImGui::Begin("Loading Screen", nullptr, flags);

  std::stringstream progstr;
  progstr << idx << "/" << graph.size(); 
  float progress = float(idx) / float(graph.size());

  ImGui::Text("Parsing Documents... Please Wait");

  ImGui::ProgressBar(progress, ImVec2(0.f,0.f), progstr.str().c_str()); 
   
  graph.getChild(idx)->parseReferences();
  idx++;
  
  ImGui::End();
  
  if( idx >= graph.size() ) return true;

  return false;
}


/**
 * @brief Display A Menu Bar For The Program. Allows User to exit
 */
bool menuBar() {
  bool close = false;

  // static bool help_selected = false;
  static bool ref_res = false; // Request ReferenceResolver Help
  
  if( ImGui::BeginMainMenuBar() ){
    if( ImGui::BeginMenu("DocManager") ){
      
      if( ImGui::MenuItem("Exit", nullptr) ) {
        close = true;
      }

      ImGui::EndMenu();
    }
    if( ImGui::BeginMenu("Help") ){


      if( ImGui::MenuItem("Reference Resolver", nullptr) ){
        ref_res = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
  
  if( ref_res ){
    if( helpRefResolve() ) {
      ref_res = false;
    }
  }

  return close;
}

/**
 * @brief Display the help dialogue for the Reference Resolver
 *
 * @returns True when the user has exited the dialogue. Else false
 */
bool helpRefResolve() {
  
  // Set Flags Such that the window cannot be moved, collapsed, nor resized
  static ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | 
                                  ImGuiWindowFlags_Popup |
                                  ImGuiWindowFlags_NoCollapse;

  // Default is uninteracted
  bool response = false;

  const string id = "Reference Resolver Help";

  // Define Popup To Open
  ImGui::OpenPopup(id.c_str());


  ImVec2 disppos; // Position To Display Popup
  { // Calculate Position on right side of screen
    auto vp = ImGui::GetMainViewport();
    auto size = vp->WorkSize;
    auto pos = vp->WorkPos;

    disppos = {pos.x + size.x, pos.y};
  }
  // Set Window
  ImGui::SetNextWindowPos(disppos, ImGuiCond_Appearing, ImVec2(1.f, 0.f));

  if( ImGui::BeginPopupModal(id.c_str(), NULL, flags) ){
    ImGui::Text("Welcome To The Help Page For The Reference Resolver");
    ImGui::Separator();
    ImGui::Separator();
    ImGui::TextWrapped(
        "\tThe Reference Resolver is used to ensure that references added to "
        "documentation are parsed correctly by DocManager. It will run you through all the "
        "documents that it found, and resolve as many references for each document as it "
        "can.");
    ImGui::Separator();
    ImGui::Separator();
    ImGui::TextWrapped(
        "\tA progress bar is displayed at the very top of the screen. This bar is used to "
        "indicate how many documents have been reviewed and how many are left. Under "
        "this is the name of the current document. Under that is another progress bar "
        "indicating how many references are left on the current document. If the current "
        "document has no references, no progress bar is displayed, and the rest of the "
        "page is not displayed either.");
    ImGui::Separator();
    ImGui::Separator();
    ImGui::TextWrapped(
        "\tOnto document reference selection. Under the second progress bar (labeled "
        "\"References Checked\") is a line indicating which reference string is being "
        "searched for. Under that is the total number of documents which might match "
        "the reference. By default, the best match is selected. Click on the drop-down "
        "menu labeled \"Pick one\" and choose the document that matches the reference. "
        "If no documents match the reference, simply click the \"No Documents Match\" "
        "button to skip to the next reference. This will add the current reference to "
        "the unfound references array for the document. To submit the chosen document, "
        "click the button labeled \"Select Reference Document\". Confirm that the "
        "chosen document is correct in the popup menu, and you're done!");
    ImGui::Separator();
    ImGui::Separator();
    ImGui::TextWrapped(
        "\tAt any time, you may skip the current document using the button labeled "
        "\"Skip To Next Document\". Note that this does not autofill any references. ");

    if( ImGui::Button("close") )
      response = true;

    ImGui::EndPopup();
  }
  

  return response;
}

/**
 * @brief Returns whether the popup was answered, answer saved to selection
 *
 * @param doc The document found to match the reference
 * @param ref The reference string to match
 * @param selection The choice selected by the user
 * @returns True when the user responds to the popup, false otherwise
 */
bool correctDocPopUp(std::shared_ptr<document> doc, string ref, bool& selection) {
  // Set Flags Such that the window cannot be moved, collapsed, nor resized
  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | 
                                  ImGuiWindowFlags_NoMove | 
                                  ImGuiWindowFlags_NoCollapse;

  // Default is uninteracted
  bool response = false;

  const string id = "Confirm Document Selection";

  // Define Popup To Open
  ImGui::OpenPopup(id.c_str());


  auto center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if( ImGui::BeginPopupModal(id.c_str(), NULL, flags) ){

    ImGui::Text("Please Confirm That The Given Reference And Document Match:");
    ImGui::Separator();
    ImGui::Text("Reference: %s", ref.c_str());
    ImGui::Text("Chosen Document: %s", doc->filename().c_str());

    if( ImGui::Button("Correct") ){
      selection = true;
      response = true;
    }

    ImGui::SameLine();

    if( ImGui::Button("NOT THE SAME!") ){
      selection = false;
      response = true;
    }

    ImGui::EndPopup();
  }
  

  return response;
}

/**
 * @brief Display The Reference Heirarchies
 */
bool graphWindow(docgraph &graph) {
  (void)graph; 
  return false;
}

/**
 * @brief Display Reference Resolver window
 *
 * @param graph The graph to resolve references for
 * @returns True if the main program should exit. 
 */
bool referenceWindow(docgraph& graph) {
  static auto it = graph.begin();
  static vector<string> refs;
  bool close = false;
  static bool confirm_doc = false;

  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;

  // Set Next Window Size
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
    
  ImGui::Begin("My First Window", nullptr, flags);

  float max = (float)std::distance(graph.begin(), graph.end());
  float cur = (float)std::distance(it, graph.end());

  float progress = (max - cur) / max; 

  std::stringstream progstr;
  progstr << std::setprecision(0) << max-cur << "/" << max;

  ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), progstr.str().c_str()); 
  ImGui::SameLine();
  ImGui::Text("Documents Reviewed");


  // Check If We're At The last Document
  if( it == graph.end() ){
    ImGui::Text("No More Documents To Review");
  }else{ // Review This Document

    // Display Document Info
    ImGui::Text("Document: %s", (*it)->filename().c_str()); 

    // Static Data
    static int current_ref_idx = 0;

    // Get References
    if( refs.empty() )
      refs = (*it)->getParsedReferences();

    // If References Don't Exist, Next Doc
    if( refs.empty() ){
      ImGui::Text("Document Has No References");
    }else{ // Document has references
      float progress = float(current_ref_idx) / float(refs.size()); 
      progstr.str("");
      progstr << current_ref_idx << "/" << refs.size();
      ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), progstr.str().c_str());
      ImGui::SameLine();
      ImGui::Text("References Checked");

      ImGui::Text("Looking For Document Matching Reference \"%s\"", refs[current_ref_idx].c_str());
    
      // Possible Documents That Match The Reference
      static vector<shared_ptr<document>> poss_refs;
      static int poss_ref_idx = 0;

      // Check If First Init Of Poss_Refs
      if( poss_refs.empty() ){
        // Fill
        poss_refs = graph.getDoc(refs[current_ref_idx], 5);
      }
      // If No Possible References Exist, Continue
      if( poss_refs.empty() ){
        ImGui::Text("No Documents Match The Reference");

        // Add the reference to unFound references
        if( ImGui::Button("Add To UnFound References") ){
          (*it)->addReference(refs[current_ref_idx]);
          current_ref_idx++;
          poss_ref_idx = 0;
          if( current_ref_idx == (int)refs.size() ){
            refs.clear();
            it++;
            current_ref_idx = 0;
          }
        }
        if( ImGui::Button("Skip To Next Reference") ){
          poss_ref_idx = 0;
          current_ref_idx = 0;
          refs.clear();
          it++;
        }
      }else{ // Reference Has Possible Matching Documents
        ImGui::Text("Reference Has %ld Possible Documents", poss_refs.size());
        ImGui::Separator();
        ImGui::TextWrapped("Select The Document That Matches The Reference:");
        if( ImGui::BeginCombo("Pick One", poss_refs[poss_ref_idx]->filename().c_str()) ){
          for( int n  = 0; n < (int)poss_refs.size(); n++ ){
            if( ImGui::Selectable(poss_refs[n]->filename().c_str(), n == poss_ref_idx) )
              poss_ref_idx = n;
            if( n == current_ref_idx )
              ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }
        if( ImGui::Button("Select Reference Document")){
          confirm_doc = true;
        }
        
        if( confirm_doc ){
          bool selection;
          if( correctDocPopUp(poss_refs[poss_ref_idx], refs[current_ref_idx], selection) ){
            if( selection ){
              (*it)->addReference(poss_refs[poss_ref_idx]);
              poss_ref_idx = 0;
              poss_refs.clear();
              current_ref_idx++;
              if( current_ref_idx == (int)refs.size() ){
                refs.clear();
                it++;
                current_ref_idx = 0;
              }
            }
            confirm_doc = false;
          }
        }


        ImGui::SameLine();
        if( ImGui::Button("No Reference Documents Match") ){
          (*it)->addReference(refs[current_ref_idx]);
          poss_ref_idx = 0;
          poss_refs.clear();
          current_ref_idx++;
          if( current_ref_idx == (int)refs.size() ){
            refs.clear();
            it++;
            current_ref_idx = 0;
          }
        }
      }
      
      
    }
    string nexttxt = "Skip To Next Document";
    if( refs.empty() )
      nexttxt = "Next Document";
    if( ImGui::Button(nexttxt.c_str()) ) {
      if( !refs.empty() )
        refs.clear();
      it++;
    }
  }

  ImGui::Separator();

  if( ImGui::Button("Close Document Manager") )
    close = true;
  
  ImGui::End();

  return close;

}

