#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>


std::string RemoveChars(const std::string& source, const std::string& chars) {
   std::string result="";
   for (unsigned int i=0; i<source.length(); i++) {
      bool foundany=false;
      for (unsigned int j=0; j<chars.length() && !foundany; j++) {
         foundany=(source[i]==chars[j]);
      }
      if (!foundany) {
         result+=source[i];
      }
   }
   return result;
}


float x_offset = 0.49999622;
float y_offset = 1051.8622;

double one_cm_radius = 0.06697406 / 0.2;

double point_sample_radius = 0.06697406;


std::string color_string = "FF00FF";

float opacity_per_point = 0.05;
unsigned current_circle_id = 0;
unsigned current_text_id = 0;
unsigned tspan_id = 0;

std::string circle_header[9];

std::string percentil_text_header[10];


void load_percentil_text_chunks(std::string const& percentil_text_chunk_path) {
  std::ifstream percentile_text_chunk_file(percentil_text_chunk_path.c_str());
  
  std::string line_buffer;
  unsigned percentile_text_chunk_id = 0;
  while(getline(percentile_text_chunk_file, line_buffer)) {
    percentil_text_header[percentile_text_chunk_id++] = line_buffer;
  }

  percentile_text_chunk_file.close();
}

void load_circle_chunks(std::string const& circle_chunk_path) {
  std::ifstream circle_chunk_file(circle_chunk_path.c_str());
  
  std::string line_buffer;
  unsigned circle_chunk_id = 0;
  while(getline(circle_chunk_file, line_buffer)) {
    circle_header[circle_chunk_id++] = line_buffer;
  }

  circle_chunk_file.close();
}

/*
std::string circle_header_p1 = ;
//now put opacity value [0 points..MAX points per gridpoint] -> [0.0..1.0]
std::string circle_header_p2_black =;
std::string circle_header_p2_red =;
//now put number as id starting from somewhere (0 will be fine)
std::string circle_header_p3 = "\"\n  cx=\"";
//now put x_offset + x_pos
std::string circle_header_p4 = "\"\n  cy=\"";
//now put y_offset - y_pos
std::string circle_header_p5 = "\"\n  r=\"";
//now put radius of circle
std::string circle_header_p6 = "\" />\n";
*/
/*     id="circle4163"
       cx="15.500001"
       cy="1036.8622"
       r="0.36697406" />
*/

int main(int argc, char** argv) {

  if(argc < 2) {
    std::cout << "Please specify at least one test log-file\n";
    return -1;
  }

  load_percentil_text_chunks("percentile_text_definition_fragments.svg");
	load_circle_chunks("circle_definition_fragments.svg");

  std::ifstream svg_header_ifs("svg_plot_header.head");
  std::string svg_header_string ( (std::istreambuf_iterator<char>(svg_header_ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

  svg_header_ifs.close();

  float x_start_handy_pos = std::atof(argv[argc-6]);
  float y_start_handy_pos = std::atof(argv[argc-5]);
  float x_handy_offset = std::atof(argv[argc-4]);
  float y_handy_offset = std::atof(argv[argc-3]);
  int positions_per_x = std::atoi(argv[argc-2]);

  unsigned int current_handy_position = 0;

  std::ofstream svg_plot_file(argv[argc-1]);

  svg_plot_file << svg_header_string;


  for(int file_idx = 1; file_idx < argc-6; ++file_idx) {
 
  //std::string filename(argv[file_idx]);

  std::ifstream log_file(argv[file_idx]);

  std::string one_line;

  unsigned int extra_info_linecount = 0;

  double avg_pos_x;
  double avg_pos_y;

  while(std::getline(log_file, one_line) ) {
    //data point line

    size_t first_open_par = one_line.find_first_of('(');
    if( first_open_par == 0) {

      one_line = RemoveChars(one_line, "(),");
      /*one_line.erase(std::remove_if(one_line.begin(),
				    one_line.end(),
				    &is_unwanted_character) );*/
      //std::cout << "New line: " << one_line << "\n";

      std::istringstream position_components(one_line);

      float compx, compy;
      position_components >> compx;
      position_components >> compy;
      compx *= 100.0;
      compy *= 100.0;
      std::cout << "."<< compx << ".." << compy <<"\n";
      float point_pos_x = x_offset + compx;
      float point_pos_y = y_offset - compy;
      //std::cout << svg_header_string << "\n";
          svg_plot_file 
            << circle_header[0]
            << 0.05
            << circle_header[1]
            << "0000FF"
            << circle_header[2]
            << 1
            << circle_header[3]
            << "0000FF"
            << circle_header[4]
            << current_circle_id++
            << circle_header[5]
            << point_pos_x
            << circle_header[6]
            << point_pos_y
            << circle_header[7]
            << point_sample_radius
            << circle_header[8]
            <<"\n";
    } else {
	std::cout << "Parsing more..\n";
	if( 0 == extra_info_linecount) {
	  std::string avg_pos_string = one_line.substr(first_open_par);
	 // std::cout << "AVG_n_rm:" << avg_pos_string << "\n";
          avg_pos_string = RemoveChars(avg_pos_string , "(),");
	 // std::cout << "AVG:" << avg_pos_string << "\n";
	  std::istringstream position_components(avg_pos_string);

	  float compx, compy;
	  position_components >> compx;
	  position_components >> compy;
	  compx *= 100.0;
	  compy *= 100.0;

	  avg_pos_x = x_offset + compx;
	  avg_pos_y = y_offset - compy;

          svg_plot_file 
            << circle_header[0]
            << 0.5
            << circle_header[1]
            << "FF0000"
            << circle_header[2]
            << 1
            << circle_header[3]
            << "FF0000"
            << circle_header[4]
            << current_circle_id++
            << circle_header[5]
            << avg_pos_x
            << circle_header[6]
            << avg_pos_y
            << circle_header[7]
            << point_sample_radius
            << circle_header[8]
            <<"\n";
	  std::cout << "AVERAGE POS: " << compx << ", " << compy << "\n";
        } else if( 1 == extra_info_linecount) {
	  std::cout << "In second branch\n";
	  size_t first_double_colon = one_line.find_first_of(':');
	  std::string avg_pos_string = one_line.substr(first_double_colon+1);
	  std::cout << "AVG_n_rm:" << avg_pos_string << "\n";
          avg_pos_string = RemoveChars(avg_pos_string , "(),");
	  std::cout << "AVG:" << avg_pos_string << "\n";
	  std::istringstream position_components(avg_pos_string);

	  float percentil_distance;
	  position_components >> percentil_distance;
	
	  std::cout << "PERCENTIL: " << percentil_distance << "\n";
        } else if ( 2 == extra_info_linecount) {
	  size_t first_double_colon = one_line.find_first_of(':');
	  std::string avg_pos_string = one_line.substr(first_double_colon+1);
	  std::cout << "AVG_n_rm:" << avg_pos_string << "\n";
          avg_pos_string = RemoveChars(avg_pos_string , "(),");
	  std::cout << "AVG:" << avg_pos_string << "\n";
	  std::istringstream position_components(avg_pos_string);

	  float percentil_distance;
	  position_components >> percentil_distance;

	  std::cout << "PERCENTIL: " << percentil_distance << "\n";

          svg_plot_file 
            << circle_header[0]
            << 0.5
            << circle_header[1]
            << "FF0000"
            << circle_header[2]
            << 0
            << circle_header[3]
            << "FF0000"
            << circle_header[4]
            << current_circle_id++
            << circle_header[5]
            << avg_pos_x
            << circle_header[6]
            << avg_pos_y
            << circle_header[7]
            << percentil_distance
            << circle_header[8]
            <<"\n";


         svg_plot_file
           << percentil_text_header[0]
           << avg_pos_x
           << percentil_text_header[1]
           << avg_pos_y -(percentil_distance + 1.56509)
           << percentil_text_header[2]
           << current_text_id++
           << percentil_text_header[3]
           << tspan_id
           << percentil_text_header[4]
           << avg_pos_x
           << percentil_text_header[5]
           << avg_pos_y -(percentil_distance + 1.56509)
           << percentil_text_header[6]
           << tspan_id+1
           << percentil_text_header[7]
           << 90
           << percentil_text_header[8]
           << std::fixed << std::setprecision(2) << percentil_distance 
           << percentil_text_header[9]
           << "\n";

           tspan_id += 2;

          float ground_truth_position_x = (x_start_handy_pos + (current_handy_position % positions_per_x) * x_handy_offset) * 100.0;
          float ground_truth_position_y = (y_start_handy_pos + (current_handy_position / positions_per_x) * y_handy_offset) * 100.0;
          std::cout << ground_truth_position_y << "xxxx\n";

           svg_plot_file
             << "<path style=\"fill:none;fill-rule:evenodd;stroke:#FF0000;stroke-width:0.26605198;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker-end:url(#Arrow1Send)\" d=\"M " << 
          avg_pos_x << "," <<
          avg_pos_y <<" " <<
          std::to_string(x_offset + ground_truth_position_x) <<"," 
       << std::to_string(y_offset - ground_truth_position_y) <<"\" id=\"path51522\" inkscape:connector-curvature=\"0\" sodipodi:nodetypes=\"cc\" />\n";
	
          ++current_handy_position;
  }


	extra_info_linecount++;
    }
  }

  log_file.close();
  }

  svg_plot_file << "</g>\n</svg>";
  svg_plot_file.close();
  return 0;
}
