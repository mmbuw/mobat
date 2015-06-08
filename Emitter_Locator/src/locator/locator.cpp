#ifndef LOCATOR_CPP
#define LOCATOR_CPP

#include "locator.hpp"

void Locator::update_times(double a, double b, double c, double d)
{
	m_mics[0].set_toa(a);
  	m_mics[1].set_toa(b);
  	m_mics[2].set_toa(c);
  	m_mics[3].set_toa(d);
};


double Locator::betrag(glm::vec2 const& a, glm::vec2 const& b) const
{
	return(sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
};

double Locator::dif(glm::vec2 const& p, microphone const& mic1, microphone const& mic2) const
{
	double dtoa = (mic1.m_toa - mic2.m_toa);
	glm::vec2 p_mic_1{mic1.m_x, mic1.m_y};
	glm::vec2 p_mic_2{mic2.m_x, mic2.m_y};
	//std::cout<<"betr "<<betrag(p , p_mic_1)<<"    "<<betrag(p , p_mic_2)<<"    "<<c*dtoa<<"\n";
	return this->betrag(p , p_mic_1) - this->betrag(p , p_mic_2) - m_c * dtoa;
};

glm::vec2 Locator::locate() const
{
	int min_x = -1;
	int min_y = -1;
	double min_dif = 1000000;
	double temp_dif = 0;
	glm::vec2 test;
	for(int x = 0; x <= 200; ++x/*x=x+10*/ )
	{
		test.x =x;
		for(int y = 0; y <= 100; ++y/*y=y+10*/)
		{
			test.y = y;
			for(int i = 0; i < 3; ++i)
			{
				for(int j = i+1; j < 4; ++j)
				{
					//std::cout<<fabs(dif(test, mics[i], mics[j]))<<"   ";
					temp_dif += fabs(this->dif(test, m_mics[i], m_mics[j]));
				}
			}
			
	
			if(fabs(temp_dif) < min_dif)
			{
				min_dif = fabs(temp_dif);
				min_x = x;
				min_y = y;
			}
			temp_dif = 0;
		}
	}
	//std::cout<<"("<<min_x<<", "<<min_y<<")   "<<min_dif<<"\n";
	return {min_x, min_y};
};



#endif