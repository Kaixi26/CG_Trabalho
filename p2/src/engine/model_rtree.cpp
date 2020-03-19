#include "../engine/model_rtree.h"

// TODO: FUNCTION TO CORRECTLY DEALLOCATE CLASS

Model_rtree::Model_rtree(XMLNode* node){
    while(node){
        const char* node_name = node->ToElement()->Name();
        //printf("%s\n", node_name);
        if(!strcmp(node_name, "scene"))
            node = node->FirstChild();
        else if(!strcmp(node_name, "group")){
            add_child(new Model_rtree(node->FirstChild()));
            if(node->NextSibling())
                add_child(new Model_rtree(node->NextSibling()));
            break;
        }
        else if(!strcmp(node_name, "models")){
            for(XMLNode* tmp = node->FirstChild(); tmp; tmp = tmp->NextSibling())
                add_child(new Model_rtree(tmp));
            if(node->NextSibling())
                add_child(new Model_rtree(node->NextSibling()));
            break;
        }
        else if(!strcmp(node_name, "translate")){
            if(this->type == RTREE_UNDEFINED){
                this->type = RTREE_TRANSFORMATION;
                this->data = malloc(sizeof(Matrix));
                *((Matrix*)this->data) = Matrix::identity();
            }
            ((Matrix*)this->data)->translate(
                node->ToElement()->FloatAttribute("X", 0),
                node->ToElement()->FloatAttribute("Y", 0),
                node->ToElement()->FloatAttribute("Z", 0));
            node = node->NextSibling();
        }
        else if(!strcmp(node_name, "scale")){
            if(this->type == RTREE_UNDEFINED){
                this->type = RTREE_TRANSFORMATION;
                this->data = malloc(sizeof(Matrix));
                *((Matrix*)this->data) = Matrix::identity();
            }
            ((Matrix*)this->data)->scale(
                node->ToElement()->FloatAttribute("X", 1),
                node->ToElement()->FloatAttribute("Y", 1),
                node->ToElement()->FloatAttribute("Z", 1));
            node = node->NextSibling();
        }
        else if(!strcmp(node_name, "rotate")){
            if(this->type == RTREE_UNDEFINED){
                this->type = RTREE_TRANSFORMATION;
                this->data = malloc(sizeof(Matrix));
                *((Matrix*)this->data) = Matrix::identity();
            }
            ((Matrix*)this->data)->rotate(
                node->ToElement()->FloatAttribute("angle", 0) * (M_PI/180.0f),
                node->ToElement()->FloatAttribute("axisX", 0),
                node->ToElement()->FloatAttribute("axisY", 0),
                node->ToElement()->FloatAttribute("axisZ", 0));
            node = node->NextSibling();
        }
        else if(!strcmp(node_name, "model")){
            this->type = RTREE_MODEL;
            this->data = new Model(node->ToElement()->Attribute("file"));
            break;
        }
        else node = node->NextSibling();
    }
}

void Model_rtree::add_child(Model_rtree* child){
    if(this->nextchild >= this->size){
        this->size *= 2;
        this->children = (Model_rtree**)reallocarray((void*)this->children, sizeof(Model_rtree*), this->size);
    }
    this->children[this->nextchild++] = child;
}

void Model_rtree::printAux(int depth){
    for(int i=0; i<depth; i++)
        printf("\t");
    if(this->type == RTREE_UNDEFINED) printf("UNDEFINED\n");
    else if(this->type == RTREE_TRANSFORMATION){
        printf("TRANSFORMATION ");
        ((Matrix*)this->data)->print();
        printf("\n");
    }
    else if(this->type == RTREE_MODEL) {
        printf("MODEL ");
        printf("%d %s\n", ((Model*)this->data)->vertex_nr, ((Model*)this->data)->filename);
    }
    for(int i=0; i<this->nextchild; i++)
        children[i]->printAux(depth+1);
}
void Model_rtree::print(){
    this->printAux(0);
}

void Model_rtree::draw(){
    this->draw((const draw_opts){});
}


void Model_rtree::draw(const draw_opts draw_opts){
    if(this->type == RTREE_TRANSFORMATION){
        glPushMatrix();
        glMultMatrixf((float*)((Matrix*)this->data)->v);
        for(int i=0; i<this->nextchild; i++)
            this->children[i]->draw(draw_opts);
        glPopMatrix();
    }
    else if(this->type == RTREE_MODEL)
        ((Model*)this->data)->draw(draw_opts);
    else if(this->type == RTREE_UNDEFINED)
        for(int i=0; i<this->nextchild; i++)
            this->children[i]->draw(draw_opts);
}