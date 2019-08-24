#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <cmath>

using namespace std;

struct st_pos
{
    st_pos()
    {
        x=y=0.0;
    }
    st_pos(float _x,float _y,float _z)
    {
        x=_x;
        y=_y;
        z=_z;
    }
    /*st_pos(st_pos& _pos)
    {
        x=_pos.x;
        y=_pos.y;
    }*/

    float x,y,z;

    st_pos operator-(st_pos _pos)
    {
        return st_pos(x-_pos.x,y-_pos.y,z-_pos.z);
    }
    st_pos operator-=(st_pos _pos)
    {
        x-=_pos.x;
        y-=_pos.y;
        z-=_pos.z;

        return *this;
    }
    st_pos operator+(st_pos _pos)
    {
        return st_pos(x+_pos.x,y+_pos.y,z+_pos.z);
    }
    st_pos operator+=(st_pos _pos)
    {
        x+=_pos.x;
        y+=_pos.y;
        z+=_pos.z;

        return *this;
    }
    st_pos operator*(float scale)
    {
        return st_pos(x*scale,y*scale,z*scale);
    }
    st_pos operator*=(float scale)
    {
        x*=scale;
        y*=scale;
        z*=scale;

        return *this;
    }
    st_pos operator/(float scale)
    {
        return st_pos(x/scale,y/scale,z/scale);
    }
    st_pos operator/=(float scale)
    {
        x/=scale;
        y/=scale;
        z/=scale;

        return *this;
    }
    bool operator==(st_pos _pos)
    {
        return (x==_pos.x && y==_pos.y && z==_pos.z);
    }
    float distance3(st_pos _pos)
    {
        return ( (x-_pos.x)*(x-_pos.x)+(y-_pos.y)*(y-_pos.y)+(z-_pos.z)*(z-_pos.z) );
    }
    float distance(st_pos _pos)
    {
        return sqrt(distance3(_pos));
    }
    float length(void)
    {
        return sqrt( x*x+y*y+z*z );
    }
    float normalize()
    {
        float length=sqrt( x*x+y*y+z*z );
        x/=length;
        y/=length;
        z/=length;

        return length;
    }

};

string float_to_pdb_string(float value);

int main()
{
    cout<<"SAXS Model Scaling\n\n";
    cout<<"Name of File: ";
    string input,input_file_name;
    while(true)
    {
        getline(cin,input);

        //test if file exists
        ifstream file(input.c_str());
        if(file==0)
        {
            cout<<"\nERROR: Could not find that file\n";
            file.close();
        }
        else
        {
            file.close();
            break;
        }
    }
    input_file_name=input;

    cout<<"\nEnter Scale Factor: ";
    float scale_val=0;
    while(true)
    {
        getline(cin,input);

        //test if value is valid
        float value=atof(input.c_str());

        if(value<=0)
        {
            cout<<"\nERROR: Value not valid\nEnter Scale Factor: ";
        }
        else
        {
            scale_val=value;
            break;
        }
    }

    //create output file
    string output_file_name(input_file_name,0,(int)input_file_name.size()-4);
    output_file_name.append("_scaled_");
    stringstream ss;
    ss<<scale_val;
    output_file_name.append(ss.str());
    output_file_name.append("x.pdb");
    ofstream output_file(output_file_name.c_str());
    if(output_file==0)
    {
        cout<<"ERROR: Could not create an output file\n";
        return 1;
    }

    //copy old data to the new file
    ifstream input_file(input_file_name.c_str());
    string word,line;
    while(getline(input_file,line))
    {
        //copy if not starts with ATOM
        if(line[0]=='A'&&line[1]=='T'&&line[2]=='O'&&line[3]=='M')
        {
            float pos[3]={0,0,0};
            //read and scale position
            pos[0]=atof(string(line,30,8).c_str())*scale_val;
            pos[1]=atof(string(line,38,8).c_str())*scale_val;
            pos[2]=atof(string(line,46,8).c_str())*scale_val;

            //convert to strings and reformat to length 8
            //replace with new values
            string x_string=float_to_pdb_string( pos[0] );
            string y_string=float_to_pdb_string( pos[1] );
            string z_string=float_to_pdb_string( pos[2] );
            int line_pos_x=30;
            int line_pos_y=38;
            int line_pos_z=46;
            for(int i=0;i<8;i++)
            {
                line[line_pos_x+i]=x_string[i];
                line[line_pos_y+i]=y_string[i];
                line[line_pos_z+i]=z_string[i];
            }
        }

        //copy to new file
        output_file<<line<<endl;
    }

    cout<<"\nComplete: A scaled file have been created\n\nPress [ENTER] to Exit\n";
    getline(cin,input);

    return 0;
}

string float_to_pdb_string(float value)
{
    //return string("   1.000");

    if(value>=10000.000) return string("  ERROR ");
    if(value<=-1000.000) return string("  ERROR ");

    //rounding
    if(value>0) value+=0.0005;
    if(value<0) value-=0.0005;

    //value convert to strings and reformat to length 8
    stringstream ss;
    ss<<(float)value;
    string string_value(ss.str());
    bool is_float=false;
    for(int i=0;i<string_value.length();i++)
    {
        if(string_value[i]=='.')
        {
            is_float=true;
            //cut if more than 3 decimals
            if((int)string_value.length()-i>4)
            {
                string cut_string=string(string_value,0,i+4);
                string_value=cut_string;
            }
            break;
        }
    }
    if(!is_float) string_value.append(".000");
    string ret_string(8,' ');
    int counter=0;
    for(int i=7;i>=0;i--)
    {
        if((int)string_value.size()-1-counter<0)
        {
            //space fill
            ret_string[i]=' ';
        }
        else ret_string[i]=string_value[ (int)string_value.size()-1-counter ];
        counter++;
    }

    return ret_string;
}
