#include "trim_adc.hxx"
#include "TString.h"
#include "string"
#include "dirent.h"

TString filename_data;
std::vector<string> file_names;
DIR *fdir;
struct dirent *entry;

// ------------------------ This is the analysis function --------------------------

void Analysis(){
  
  trim_adc* sts = new trim_adc(filename_data); 
 
  // ............... oooo00000oooo........................
  //! Variables for reading, analyzing data and displaying histograms
  int ch_min = 0;
  int ch_max = 127;
  int ch_step =1;
  
  int grp_min = 0;
  int grp_max = 4;
  int grp_step = 1;
  
  int d_min = 0;
  int d_max = 30; // ADC counters are prepared for 0->30. 31th is fast counter.
  int d_step = 1;
  
  // The following values specify the scan range in the input text file.
  int vp_min = 0;
  int vp_max = 255;
  //int vp_step = 4;
  int vp_step = sts->get_vpstep();
  
  // this value determines where to cut the double pulses, basically by forcing the data to stop in 200.
  // it has to be checked with the value of pulses in jected in the check trim.
  // if you want to see everything, please, put this value at least twice larger than the number of injected pulses
  int cut_db_pulses = 200;
  
  // ............... oooo00000oooo........................
  //! Fitting windows variables window (Like in the previous) This will create the arrange for the cuttings.
  // width determines the range to look for the peak (MEAN +/- width) 
  // KAZ: 
  float amp_cal_min = 20.;   
  float amp_cal_max = 240.;
  int width =35;
  
//  int dcut_min_user = 6;    // start counting from 0
//  int dcut_max_user = 26;     // Largest possible value = 30
  int dcut_min_user = 0;    // start counting from 0
  int dcut_max_user = 30;     // Largest possible value = 30
  
  // ............... oooo00000oooo........................
  //! Test channels and channels for displaying histograms
  // a test channel for displaying histos and set of channels for printing out noise levels
  int test_ch =48;
  int grp_sel = 0;
  int ch_comp[10] = {105, 40, 59, 64, 6, 91, 94, 101, 110, 121};
  
  bool read_fast = true;
  
  // check if there is not root file created
  if (!(sts->Check_root_file())) return;
  
  // initialized the settings described above
  if (!(sts->Init_ch(ch_min,ch_max,ch_step))) return;
  if (!(sts->Init_grp(grp_min,grp_max,d_step))) return;
  if (!(sts->Init_d(d_min,d_max,grp_step))) return;
  if (!(sts->Init_vp(vp_min,vp_max,vp_step))) return;

  // Fitting windows function
  sts->Fitting_windows(amp_cal_min,amp_cal_max,width);
 
  // Create histograms to be called along the programm execution
  sts->Create_histo();

  if (!(sts->Reading_file(cut_db_pulses))) return;

  // Analysis function in .cxx 
  // Analysis (cut_db_pulses, width, discriminators to exclude min, discriminator to exclude max, test channel to display, ADC noise fitting, ADC noise calc, find noise fast disc)
  sts->Analysis(cut_db_pulses, width, dcut_min_user, dcut_max_user, test_ch, true, true, read_fast);
  
  // Display everything. 
  cout << "Display_histo_adc" << endl;
  sts->Display_histo_adc(width, dcut_min_user, dcut_max_user, ch_comp, grp_sel);

    if (read_fast == true) {
      cout << "Display_histo_fast" << endl;
      sts->Display_histo_fast(width,ch_comp);
    }
  cout << "Display_values" << endl;
  sts->Display_values(ch_comp);
  cout << "Close_root_file" << endl;
  sts->Close_root_file();

}

// ............... oooo00000oooo........................
//! Reading file that contains test_files names
// This function reads a file that contains the name of the files where the measured values are stored
bool Read_file_tests(){
  // Path of the file with name hmodule_lab_test
  // h.txt is the name of the file mentioned above
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/pogo_pin/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/temp_tests/feb-b_9/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/conv_test2/pscan_files/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup1/long_run/a_holes";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup1/long_run1.1/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/bef_aft/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/cables_files/kit_cables/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/csa_currents/pscan_files/feb_b";
  //TString file_list = "/home/paper-plane/CBM/test_beam_feb18/pscan_files/a";
  
  // -------------------------------------------------------------------------------------------
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/pscan_files_modules/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/pscan_feb_8/a";
  //TString file_list = "/home/paper-plane/CBM/test_beam_feb17/radiationTestResults/p_scan_files/a";
  //TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/mSTS/02Tl/pscan_files/a";
  TString file_list = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/mSTS/assembled/pscan_files/a";
  
  ifstream myfile;
  myfile.open(file_list+ ".txt");
  std::string line;
  std::string delimiter = ".txt";
  size_t pos = 0;
  std::string token;

 // reading list file and storing it in a vector
  cout<<"......................oooo00000oooo..............................."<<endl;
  while(!myfile.eof()){
    std::getline(myfile, line);
    if (line.empty()) continue;
     else {
    while ((pos = line.find(delimiter)) != std::string::npos) {
      token = line.substr(0, pos);
      cout << token << endl;
      line.erase(0, pos + delimiter.length());
      }  
      file_names.push_back(token);
     }
    }

  cout<<file_names.size()<<endl;  
  
  if (file_names.size()!=0) return false;
  return true;
   
}

// ............... oooo00000oooo........................
//! Get_file_name from the file list
std::string Get_file_name(int i) {return file_names[i];}



// ............... oooo00000oooo........................
//! Executing the analysis 
int execution(const char* filename){
  TString f = filename;
  int ipos = f.Last('.');
  if ( ipos >= 0 ) {
    f.Remove(ipos);
  }
  filename_data = f;
  
  //filename_data = "pscan/pscan_20211102_RedFEB8";
  //filename_data = "pscan/pscan_20211204_RedFEB8_ASIC4";
  //filename_data = "pscan/pscan_redfeb8_20211212_fifo8";
  //filename_data = "pscan/pscan_redfeb8_20211212_fifo8_aftercalib";
  
//filename_data = "pscan/pscan_20211107_RedFEB8_trimof127_0_15_to_110";
  //filename_data = "pscan/pscan_20211114_BlueFEB8_wunconnected_cable";
  //filename_data = "pscan/pscan_20211114_BlueFEB8_wunconnected_cable_SMX4";
  //filename_data = "pscan/pscan_20211114_BlueFEB8_wunconnected_cable_SMX4_2";
  //filename_data = "pscan_samples/pscan_190308_1533_sn_008_asic_addr_4_040_028_055_186_vp_000_255_040_holes"; // ommit .txt as it will be added in the class trim_adc.


  Analysis();
  return 0;
}

// ............... oooo00000oooo........................
//! Executing the analysis 
int execution_multi(const char* filedir){
  // Here is the directory of the test files, same place where root files will be created
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup1/glob_top/russ_febs/vref_t_scan/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/temp_tests/feb-b_9/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/pogo_pin/";
  
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/summer_std/pscan_files/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/sh_time/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/csa_currents/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_b_setup2/cables_files/kit_cables/";
  //TString dir = "/home/paper-plane/CBM/test_beam_feb18/pscan_files/";
  
  // --------------------------------------------------------------------------------------
  
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/pscan_feb_8/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/pscan_files_modules/";
  //TString dir = "/home/paper-plane/CBM/test_beam_feb17/radiationTestResults/p_scan_files/";
  //TString dir = "/home/paper-plane/CBM/test/feb_b/feb_8_setup/mSTS/02Tl/pscan_files/";
 //TString dir = filedir;
  
  
  // reading file with list of measurements files.
  //Read_file_tests();
  // Analysis function is called for every file in the list. At the end, everything is closed and root files can be accessed via TBrowser
  //for (int i = 0; i<int(file_names.size()); i++){
   // filename_data= dir + TString(Get_file_name(i));
   // Analysis();
 // }
    fdir = opendir(filedir);

    if (fdir == NULL) {
        perror("ディレクトリを開けません");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(fdir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char filePath[256];
            snprintf(filePath, sizeof(filePath), "%s/%s",filedir, entry->d_name);
            execution(filePath);
        }
    }
    closedir(fdir);
  

  return 0;
}
