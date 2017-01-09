#ifndef DYNAMICBLUR_H_INCLUDED
#define DYNAMICBLUR_H_INCLUDED

#include <SFML/Graphics.hpp>

class DynamicBlur
{
public:
    DynamicBlur(unsigned int texureWidth, unsigned int texureHeight);

    const sf::Texture& operator()(const sf::Texture&);
    void blurRenderTexture(sf::RenderTexture& renderTexture);

    void setDownSizeFactor(unsigned int f) {__downSizeFactor = pow(2, f);}

private:
    float __downSizeFactor;

    const unsigned int __WIDTH;
    const unsigned int __HEIGHT;

    sf::RenderTexture __blurTexture;
    sf::RenderTexture __lowBlurTexture;

    sf::Shader __blur;
};

#endif // DYNAMICBLUR_H_INCLUDED
