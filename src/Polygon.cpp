/*

    ofxTableGestures (formerly OF-TangibleFramework)
    Developed for Taller de Sistemes Interactius I
    Universitat Pompeu Fabra

    Copyright (c) 2011 Daniel Gallardo Grassot <daniel.gallardo@upf.edu>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

*/

#include "Polygon.h"
#include "CollisionHelper.h"
#include "ofMain.h"
#include "Triangulate.h"

using namespace std;

namespace Figures
{
    

Polygon::Polygon()
{
    last_texture_state = false;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    linemesh.setMode(OF_PRIMITIVE_LINE_STRIP);
}

Polygon::Polygon(vector_points):processed(false),it_is_empty(true)
{
    centre = ofPoint(-1.0,-1.0);
}

void Polygon::AddVertex(ofPoint vertex)
{
    raw_vertexs.push_back(vertex);
    linemesh.addVertex(vertex);
    bbox.AddPoint(vertex.x,vertex.y);
    if(it_is_empty)
    {
        centre = ofPoint(vertex);
    }
    else
    {
        centre += vertex;
        centre = centre / 2;
    }
    processed = false;
}

void Polygon::RebuildGeometry()
{
    if(raw_vertexs.size() >= 3)
    {
        updatedVertexs.clear();
        textureVertexs.clear();
        Triangulate::Process(raw_vertexs,updatedVertexs);
        float rx = bbox.GetXmax() - bbox.GetXmin();
        float ry = bbox.GetYmax() - bbox.GetYmin();
        for(unsigned int i = 0; i < updatedVertexs.size(); i++)
        {
            textureVertexs.push_back(ofPoint( 1 - ((updatedVertexs[i].x - bbox.GetXmin())/rx), 1 - ((updatedVertexs[i].y - bbox.GetYmin())/ry) ));
        }
    }
    mesh.clear();
    int tcount = updatedVertexs.size()/3;
    for (int i=0; i<tcount; i++)
    {
        mesh.addVertex(ofVec3f(updatedVertexs[i*3+0].x,updatedVertexs[i*3+0].y));
        mesh.addVertex(ofVec3f(updatedVertexs[i*3+1].x,updatedVertexs[i*3+1].y));
        mesh.addVertex(ofVec3f(updatedVertexs[i*3+2].x,updatedVertexs[i*3+2].y));

        if(has_texture)
        {
            mesh.addTexCoord(ofVec2f(textureVertexs[i*3+0].x * texture.getTextureReference().texData.tex_t,
                                 textureVertexs[i*3+0].y * texture.getTextureReference().texData.tex_u));
            mesh.addTexCoord(ofVec2f(textureVertexs[i*3+1].x * texture.getTextureReference().texData.tex_t,
                                 textureVertexs[i*3+1].y * texture.getTextureReference().texData.tex_u));
            mesh.addTexCoord(ofVec2f(textureVertexs[i*3+2].x * texture.getTextureReference().texData.tex_t,
                                 textureVertexs[i*3+2].y * texture.getTextureReference().texData.tex_u));
        }
    }

}

int Polygon::GetTriangleNumber()
{
    if (!processed) RebuildGeometry();
    return updatedVertexs.size()/3;
}

void Polygon::Design()
{
    if (!processed || (has_texture && !last_texture_state)) RebuildGeometry();
    last_texture_state = has_texture;

    int tcount = updatedVertexs.size()/3;
    if(has_texture)
    {
        mesh.enableTextures();
        texture.getTextureReference().bind();
        mesh.draw();
        texture.getTextureReference().unbind();
    }
    else
    {
        mesh.disableTextures();
        mesh.draw();
    }

}

void Polygon::DesignStroke()
{
    linemesh.draw();
}

bool Polygon::CheckCollision(ofPoint const & point)
{
    if (!processed) RebuildGeometry();
    int tcount = updatedVertexs.size()/3;
    for (int i=0; i<tcount; i++)
    {
        if(CollisionHelper::CollideTriangle(updatedVertexs[i*3+0],updatedVertexs[i*3+1],updatedVertexs[i*3+2],point.x, point.y))
            return true;
    }
    return false;
}

void Polygon::GetCentre(float & x, float & y)
{
    x = centre.x;
    y = centre.y;
}

}
