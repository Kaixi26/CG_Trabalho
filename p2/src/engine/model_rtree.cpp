#include "../engine/model_rtree.h"
#include "catmull.h"

// TODO: FUNCTION TO CORRECTLY DEALLOCATE CLASS

XMLNode* nextNonCommentSibling(XMLNode* node){
    for(node = node->NextSibling(); node != NULL && node->ToComment() != NULL; node = node->NextSibling())
        ;
    return node;
}

XMLNode* firstNonCommentChild(XMLNode* node){
    for(node = node->FirstChild(); node != NULL && node->ToComment() != NULL; node = node->NextSibling())
        ;
    return node;
}

int isTransform(XMLNode* node){
    return node
        && !strcmp(node->ToElement()->Name(), "translate") 
        && !strcmp(node->ToElement()->Name(), "scale")
        && !strcmp(node->ToElement()->Name(), "rotate");
}

void Model_rtree::addUntilTransform(XMLNode* node){
    while(isTransform(node)){
        add_child(new Model_rtree(node));
        node = nextNonCommentSibling(node);
    }
    if(node)
        add_child(new Model_rtree(node));
}

Model_rtree::Model_rtree(XMLNode* node){
    while(node){
        if(node->ToComment() != NULL) node = node->NextSibling();

        const char* node_name = node->ToElement()->Name();

        printf("%s\n", node_name);
        if(!strcmp(node_name, "scene"))
            node = firstNonCommentChild(node);
        else if(!strcmp(node_name, "group")){
            add_child(new Model_rtree(firstNonCommentChild(node)));
            XMLNode* next = nextNonCommentSibling(node);
            if(next) add_child(new Model_rtree(next));
            break;
        }
        else if(!strcmp(node_name, "models")){
            for(XMLNode* tmp = firstNonCommentChild(node); tmp; tmp = nextNonCommentSibling(tmp)){
                add_child(new Model_rtree(tmp));
            }
            XMLNode* next = nextNonCommentSibling(node);
            if(next){
                add_child(new Model_rtree(next));
            }
            break;
        }
        else if(!strcmp(node_name, "translate")){
            if(node->ToElement()->Attribute("time")){
                this->type = RTREE_CATMULL;
                this->data = calloc(1, sizeof(Catmull));
                this->data = new Catmull();
                for(XMLNode* tmp = firstNonCommentChild(node); tmp; tmp = nextNonCommentSibling(tmp)){
                    ((Catmull*)(this->data))->insert(Vertex(
                          tmp->ToElement()->FloatAttribute("X", 0)
                        , tmp->ToElement()->FloatAttribute("Y", 0)
                        , tmp->ToElement()->FloatAttribute("Z", 0)));
                }
                addUntilTransform(nextNonCommentSibling(node));
                return;
            }
            else{
                if(this->type == RTREE_UNDEFINED){
                    this->type = RTREE_TRANSFORMATION;
                    this->data = malloc(sizeof(Matrix));
                    *((Matrix*)this->data) = Matrix::identity();
                }
                ((Matrix*)this->data)->translate(
                    node->ToElement()->FloatAttribute("X", 0),
                    node->ToElement()->FloatAttribute("Y", 0),
                    node->ToElement()->FloatAttribute("Z", 0));
                if(isTransform(nextNonCommentSibling(node)))
                    node = nextNonCommentSibling(node);
                else {
                    addUntilTransform(nextNonCommentSibling(node));
                    return;
                }
            }
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
            if(isTransform(nextNonCommentSibling(node)))
                node = nextNonCommentSibling(node);
            else {
                addUntilTransform(nextNonCommentSibling(node));
                return;
            }
        }
        else if(!strcmp(node_name, "rotate")){
                /* TODO */
            if(node->ToElement()->Attribute("time")){
                this->type = RTREE_TIMED_ROTATION;
                this->data = calloc(1, sizeof(Timed_rot));
                this->data = new Timed_rot(Vertex(
                      node->ToElement()->FloatAttribute("axisX", 1)
                    , node->ToElement()->FloatAttribute("axisY", 1)
                    , node->ToElement()->FloatAttribute("axisZ", 1)
                    ), node->ToElement()->FloatAttribute("time", 1));
                addUntilTransform(nextNonCommentSibling(node));
                return;
            }
            else {
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
                if(isTransform(nextNonCommentSibling(node)))
                    node = nextNonCommentSibling(node);
                else {
                    addUntilTransform(nextNonCommentSibling(node));
                    return;
                }
            }
        }
        else if(!strcmp(node_name, "model")){
            this->type = RTREE_MODEL;
            this->data = new Model(node->ToElement()->Attribute("file"));
            break;
        }
        else node = nextNonCommentSibling(node);
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
    else if(this->type == RTREE_CATMULL){
        printf("CATMULL ");
        ((Catmull*)this->data)->print();
        printf("\n");
    }
    else if(this->type == RTREE_TIMED_ROTATION){
        printf("ROTATION ");
        ((Timed_rot*)this->data)->print();
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
    else if(this->type == RTREE_CATMULL){
        glPushMatrix();
        Vertex curr = ((Catmull*)(this->data))->getAt(draw_opts.t);
        glTranslatef(curr.x, curr.y, curr.z);
        for(int i=0; i<this->nextchild; i++)
            this->children[i]->draw(draw_opts);
        glPopMatrix();
    }
    else if(this->type == RTREE_TIMED_ROTATION){
        glPushMatrix();
        Vertex v = ((Timed_rot*)(this->data))->v;
        float ang = ((Timed_rot*)(this->data))->calc_rot(draw_opts.t);
        glRotatef(ang, v.x, v.y, v.z);
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