#include "gui.hpp"

// ImGui
#include "imgui.h"

// C++ Includes
#include <iostream>

// using std::cout, std::endl;

/**
 * @brief Display A Menu Bar For The Program. Allows User to exit
 */
bool menuBar() {
  bool close = false;
  
  if( ImGui::BeginMainMenuBar() ){
    if( ImGui::BeginMenu("DocManager") ){
      
      if( ImGui::MenuItem("Exit", nullptr) ) {
        close = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  return close;
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

bool graphWindow(docgraph &graph) {
  (void)graph; 
  return false;
}

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
      refs = (*it)->parseReferences<WORD_XML>();

    // If References Don't Exist, Next Doc
    if( refs.empty() ){
      ImGui::Text("Document Has No References");
    }else{ // Document has references
      ImGui::Text("Looking For Document Matching Reference \"%s\"", refs[current_ref_idx].c_str());
    
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

