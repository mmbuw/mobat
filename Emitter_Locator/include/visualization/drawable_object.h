#ifndef TTT_DrawableObject_H
#define TTT_DrawableObject_H

#include <SFML/Graphics.hpp>
#include <glm/vec2.hpp>

namespace TTT{

class DrawableObject {
	public:
		DrawableObject();
		virtual ~DrawableObject();

		virtual void Draw(sf::RenderWindow& canvas) const = 0;
		//virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;
		static glm::vec2 const& getPhysTableSize();
		static void setPhysTableSize(glm::vec2 const& size);
		static void setProjection(glm::vec2 const& origin, glm::vec2 const& size);
		static void setResolution(glm::vec2 const& res);
		static void setBasis(glm::vec2 const& right, glm::vec2 const& up);

		static sf::Vector2f toProjectionSpace(glm::vec2 pos, float radius = 0.0f);
		static sf::Vector2f toProjectionSize(glm::vec2 size);
		static glm::vec2 pixelProjectionOffset();
		static glm::vec2 pixelProjectionSize();

	// protected:

		//scaling factor for the elements attributes
		static glm::vec2 physical_table_size_;
		static glm::vec2 resolution_;
		static glm::vec2 right_;
		static glm::vec2 up_;
		static glm::vec2 physical_projection_offset_;
		static glm::vec2 pixel_projection_offset_;
		static glm::vec2 physical_projection_size_;
		static glm::vec2 pixel_projection_size_;
		static float pixel_per_projection_;
		static float line_thickness_;
};

sf::Vector2f toSf(glm::vec2 const& vec);
glm::vec2 toGlm(sf::Vector2f const& vec);

}; //namespace TTT
#endif