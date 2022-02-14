// File:        imglist.cpp
// Date:        2022-01-20 03:22
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <math.h> // provides fmin and fabs functions

/**************************
* MISCELLANEOUS FUNCTIONS *
**************************/

/*
* This function is NOT part of the ImgList class,
* but will be useful for one of the ImgList functions.
* Returns the "difference" between two hue values.
* PRE: hue1 is a double between [0,360).
* PRE: hue2 is a double between [0,360).
* 
* The hue difference is the absolute difference between two hues,
* but takes into account differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
double HueDiff(double hue1, double hue2) {
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/*
* Default constructor. Makes an empty list
*/
ImgList::ImgList() {
  // set appropriate values for all member attributes here
  // create img node
  // set attributes
  northwest = NULL;
  southeast = NULL;
}

/*
* Creates a list from image data
* PRE: img has dimensions of at least 1x1
*/
ImgList::ImgList(PNG& img) {
  // build the linked node structure and set the member attributes appropriately
  // grab each pixel, set each pixel to it's specific pointer direction....
  // case 1: top left; 2 pointers to be different // instantiate 
  // case 2: normal left: 1 pointer
  // case 3: bottom left: 2 pointers
  // case 4: top normal: 1 
  // case: 5: bottom normal: 1
  // case 6: top right: 2
  // case 7: right: 1
  // case 8: bottom right: 2 // instantiate
  // ImgNode *vert;
  ImgNode *right;
  ImgNode *left;
  ImgNode *top;

  for(unsigned int x = 0; x < img.width(); x++) {
     for(unsigned int y = 0; y < img.height(); y++) {
          HSLAPixel *pixel = img.getPixel(x,y);
          ImgNode *node = new ImgNode();
          if (x == 0 && y == 0) {
            // northwest node 
            // (0, 0)
            northwest = node;
            northwest->colour = *pixel;
            left = northwest;
            right = northwest;
            top = northwest;
          } else if (x == img.width() - 1 && y == img.height() - 1) {
            // southeast node
            // (width - 1, height - 1)
            left = left->south;

            southeast = node;

            left->east = southeast;
            southeast->west = left;
            
            right->south = southeast;
            southeast->north = right;

            southeast->colour = *pixel;
          } else if (x == 0) {
            // (0, 1)

            // up/down pointers
            node->north = right;
            right->south = node;

            // moves right pointer down one node to match y of current node
            right = node;

            // colour
            node->colour = *pixel;
          } else if (y == 0) {
            // (1, 0)
            right = node;
            // set up left/right pointers
            node->west = top;
            top->east = node;

            //set to be equal to right
            left = top;
            top = node;
            
            //colour 
            node->colour = *pixel;
          } else {
            // (1, 1)

            // moves left pointer down one node to match y of current node
            left = left->south;
            
            //left/right pointer
            left->east = node;
            node->west = left;
            
            //up/down pointer
            right->south = node;
            node->north = right;

            //colour
            node->colour = *pixel;
            
            // moves right pointer down one node to match y of current node
            right = node;
          } 
       }
     }
     right = NULL;
     left = NULL;
     top = NULL;
  }


/*
* Copy constructor.
* Creates this this to become a separate copy of the data in otherlist
*/
ImgList::ImgList(const ImgList& otherlist) {
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
* Assignment operator. Enables statements such as list1 = list2;
*   where list1 and list2 are both variables of ImgList type.
* POST: the contents of this list will be a physically separate copy of rhs
*/
ImgList& ImgList::operator=(const ImgList& rhs) {
  // Re-build any existing structure using rhs as a template
  
  if (this != &rhs) { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();    
    
    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }
  
  return *this;
}

/*
* Destructor.
* Releases any heap memory associated with this list.
*/
ImgList::~ImgList() {
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
* ACCESSORS *
************/

/*
* Returns the horizontal dimension of this list (counted in nodes)
* Note that every row will contain the same number of nodes, whether or not
*   the list has been carved.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionX() const {
  // replace the following line with your implementation

  unsigned int ans = 1;
  ImgNode* node = northwest;

  while(node->east != NULL) {
    ans++;
    node = node->east;
  }
  
  return ans;
}

/*
* Returns the vertical dimension of the list (counted in nodes)
* It is useful to know/assume that the grid will never have nodes removed
*   from the first or last columns. The returned value will thus correspond
*   to the height of the PNG image from which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   y dimension.
*/
unsigned int ImgList::GetDimensionY() const {
  // replace the following line with your implementation
  unsigned int ans = 1;
  ImgNode* node = northwest;

  while(node->south != NULL) {
    ans++;
    node = node->south;
  }
  
  return ans;
}

/*
* Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
* The returned value will thus correspond to the width of the PNG image from
*   which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionFullX() const {
  // replace the following line with your implementation
  unsigned int ans = 1;
  ImgNode* node = northwest;

  while(node->east != NULL) {
    ans+=node->skipright;
    ans++;
    node = node->east;
  }
  
  return ans;
}

/*
* Returns a pointer to the node which best satisfies the specified selection criteria.
* The first and last nodes in the row cannot be returned.
* PRE: rowstart points to a row with at least 3 physical nodes
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rowstart - pointer to the first node in a row
* PARAM: selectionmode - criterion used for choosing the node to return
*          0: minimum luminance across row, not including extreme left or right nodes
*          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
*        In the (likely) case of multiple candidates that best match the criterion,
*        the left-most node satisfying the criterion (excluding the row's starting node)
*        will be returned.
* A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
* That is, a hue value of exactly 360 should be converted to 0.
* The hue difference is the absolute difference between two hues,
* but be careful about differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
  // add your implementation below
  ImgNode* curr = rowstart->east;
  ImgNode* ans = rowstart->east;
  double ansHue = 0;
  if (selectionmode == 0) {
    while(curr->east != NULL) {
      if (curr->colour.l < ans->colour.l) {
        ans = curr;
      }
      curr = curr->east;
    }
  } else if (selectionmode == 1) {
    while(curr->east != NULL) {
      double hueCurr = curr->colour.h;
      double hueWest = curr->west->colour.h;
      double hueEast = curr->east->colour.h;

      double diffWest = HueDiff(hueCurr, hueWest);
      double diffEast = HueDiff(hueCurr, hueEast);

      double diffTotal = diffWest + diffEast;
      if (diffTotal < ansHue || curr->west->west == NULL) {
        ansHue = diffTotal;
        ans = curr;
      }
      curr = curr->east;
    }
  }
  return ans;
}

/************
* MODIFIERS *
************/

/*
* Removes exactly one node from each row in this list, according to specified criteria.
* The first and last nodes in any row cannot be carved.
* PRE: this list has at least 3 nodes in each row
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: selectionmode - see the documentation for the SelectNode function.
* POST: this list has had one node removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(int selectionmode) {
  // add your implementation here
  ImgNode* row = northwest;
  while (row != NULL) {
    ImgNode* select = SelectNode(row, selectionmode);
    select->east->west = select->west;
    select->west->east = select->east;
    if (select->north == NULL && select->south != NULL) {
      select->south->north = NULL;
      select->south->skipup += (select->skipup) + 1;
    } else if (select->south == NULL && select->north != NULL) {
      select->north->south = NULL;
      select->north->skipdown += (select->skipdown) + 1;
    } else if (select->south != NULL & select->north != NULL) {
      select->south->north = select->north;
      select->north->south = select->south;
      select->south->skipup += (select->skipup) + 1;
      select->north->skipdown += (select->skipdown) + 1;
    }
    select->west->skipright = 1;
    select->east->skipleft = 1;
    delete(select);
    select = NULL;
    row = row->south;
  }
}

// note that a node on the boundary will never be selected for removal
/*
* Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
* based on specific selection criteria.
* Note that this should remove one node from every row, repeated "rounds" times,
* and NOT remove "rounds" nodes from one row before processing the next row.
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rounds - number of nodes to remove from each row
*        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
*        i.e. Ensure that the final list has at least two nodes in each row.
* POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  unsigned int newRounds = rounds;
  if (rounds > GetDimensionX() - 2) {
    newRounds = GetDimensionX() - 2;
  }
  for (unsigned int x = 0; x < newRounds; x++) {
    Carve(selectionmode);
  }
}

/*
* Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
* PRE: fillmode is an integer in the range of [0,2]
* PARAM: fillgaps - whether or not to fill gaps caused by carving
*          false: render one pixel per node, ignores fillmode
*          true: render the full width of the original image,
*                filling in missing nodes using fillmode
* PARAM: fillmode - specifies how to fill gaps
*          0: solid, uses the same colour as the node at the left of the gap
*          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
*             Note that "average" for hue will use the closer of the angular distances,
*             e.g. average of 10 and 350 will be 0, instead of 180.
*             Average of diametric hue values will use the smaller of the two averages
*             e.g. average of 30 and 210 will be 120, and not 280
*                  average of 170 and 350 will be 80, and not 260
*          2: *** OPTIONAL - FOR BONUS ***
*             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
*             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
*             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
*             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
*             Like fillmode 1, use the smaller difference interval for hue,
*             and the smaller-valued average for diametric hues
*/
PNG ImgList::Render(bool fillgaps, int fillmode) const {
  // Add/complete your implementation below
  PNG outpng;
  if (fillgaps) { // fillgaps == true
    outpng.resize(GetDimensionFullX(), GetDimensionY());  
    if (fillmode == 0) { // TODO
      ImgNode* curr = northwest;
      ImgNode* row = northwest->south;
      bool run = true;
      int x = 0;
      int y = 0;
      while (run) {
          HSLAPixel* pixel = new HSLAPixel(curr->colour.h, curr->colour.s, curr->colour.l, curr->colour.a);
          cout << curr->colour.h << endl;
          //HSLAPixel* = NULL;
          *outpng.getPixel(x, y) = *pixel;
          unsigned int skip = curr->skipright;
          if (skip != 0) {
          HSLAPixel* leftColour = pixel;
          //leftColour = NULL;
          while (skip > 0) { 
              x++;
              *outpng.getPixel(x, y) = *leftColour;
              skip--;
            }
          }
          delete pixel;
          curr = curr->east;
          x++;
          if (curr == NULL && row == NULL) {
            run = false;
          } else if (curr == NULL) {
            curr = row;
            row = row->south;
            y++;
            x = 0;
        }
      }
    } else if (fillmode == 1) { // TODO
      ImgNode* curr = northwest;
      ImgNode* row = northwest->south;
      bool run = true;
      int x = 0;
      int y = 0;
      while (run) {
          HSLAPixel* pixel = new HSLAPixel(curr->colour.h, curr->colour.s, curr->colour.l, curr->colour.a);
          *outpng.getPixel(x, y) = *pixel;
          delete pixel;
          unsigned int skip = curr->skipright;
          if (skip != 0) {  
            double avgHue;
            double avgSat;
            double avgLum;
            double avgAlp;
            double sumHue = (curr->colour.h + curr->east->colour.h); // if curr->west == NULL then can't call curr->west->colour
            if (abs(curr->colour.h + curr->east->colour.h) >= 180 && sumHue >= 360) {
              sumHue -= 360;
            }
            avgHue = sumHue / 2;
            avgSat = (curr->east->colour.s + curr->colour.s) / 2;
            avgLum = (curr->east->colour.l + curr->colour.l) / 2;
            avgAlp = (curr->east->colour.a + curr->colour.a) / 2;
            
          HSLAPixel* average = new HSLAPixel(avgHue, avgSat, avgLum, avgAlp);
          while (skip > 0) { 
              x++;
              *outpng.getPixel(x, y) = *average;
              skip--;
            }
            delete average;
          }
          curr = curr->east;
          x++;
          if (curr == NULL && row == NULL) {
            run = false;
          } else if (curr == NULL) {
            curr = row;
            row = row->south;
            y++;
            x = 0;
        }
      }
    }
  } else { // fillgaps == false
  outpng.resize(GetDimensionX(), GetDimensionY());
  ImgNode* curr = northwest;
  ImgNode* row = northwest->south;
  bool run = true;
  int x = 0;
  int y = 0;
  while (run) {
      HSLAPixel* pixel = new HSLAPixel(curr->colour.h, curr->colour.s, curr->colour.l, curr->colour.a);
      *outpng.getPixel(x, y) = *pixel;
          delete pixel;
      curr = curr->east;
      x++;
      if (curr == NULL && row == NULL) {
        run = false;
      } else if (curr == NULL) {
        curr = row;
        row = row->south;
        y++;
        x = 0;
      }
    }
  }
  return outpng;
}

/*
* Helper function deallocates all heap memory associated with this list,
* puts this list into an "empty" state. Don't forget to set your member attributes!
* POST: this list has no currently allocated nor leaking heap memory,
*       member attributes have values consistent with an empty list.
*/
void ImgList::Clear() {
  // add your implementation here
  if (northwest != NULL && southeast != NULL) {
  ImgNode* curr = northwest;
  ImgNode* front = northwest->east;
  ImgNode* row = northwest->south;
  bool run = true;
  while (run) {
      if (front == NULL && row == NULL) {
        delete curr;
        run = false;
      } else if (front == NULL) {
        delete(curr);
        curr = row;
        row = row->south;
        front = curr->east;
      } else {
        delete(curr);
        curr = front;
        front = front->east;
      }
  }
  northwest = NULL;
  southeast = NULL;
  front = NULL;
  curr = NULL;
  row = NULL;
  }
  // // ImgNode* curr = northwest;
  // // ImgNode* bottom = northwest->south;
  // // ImgNode* top = northwest->east;
  // //  while(curr->east != NULL && curr->south != NULL) {
  // //    if (bottom == NULL) {
  // //      // reset back to next col
  // //      delete(curr);
  // //      curr = top;
  // //      bottom = curr->south;
  // //      top = top->east;
  // //    } else {
  // //      // progress down in each row
  // //     delete(curr);
  // //     curr = bottom;
  // //     bottom = bottom->south;
  // //    }
  // //  }
  // //     // southeast
  // // delete(southeast);
  // // northwest = NULL;
  // // southeast = NULL;
  // // curr = NULL;
  // bottom = NULL;
  // top = NULL;
  }

/* ************************
*  * OPTIONAL - FOR BONUS *
** ************************
* Helper function copies the contents of otherlist and sets this list's attributes appropriately
* PRE: this list is empty
* PARAM: otherlist - list whose contents will be copied
* POST: this list has contents copied from by physically separate from otherlist
*/
void ImgList::Copy(const ImgList& otherlist) {
  // add your implementation here
  
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/

