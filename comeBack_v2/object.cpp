#include "object.h"

ds2::object::object(){
    _pos = vl::vec2d();
}

ds2::object::object(const vl::vec2d& pos) {
    _pos = vl::vec2d(pos);
}

vl::vec2d& ds2::object::pos(){
    return _pos;
}

const vl::vec2d& ds2::object::pos() const{
    return _pos;
}

vl::vec2d& ds2::object::vel() {
    return _vel;
}

const vl::vec2d& ds2::object::vel() const {
    return _vel;
}

double& ds2::object::mass() {
    return _mass;
}

const double& ds2::object::mass() const {
    return _mass;
}

double& ds2::object::rot()
{
    return _rot;
}

const double& ds2::object::rot() const
{
    return _rot;
}

double& ds2::object::rot_vel()
{
    return _rot_vel;
}

const double& ds2::object::rot_vel() const
{
    return _rot_vel;
}

void ds2::object::update(const double& dt)
{
    _pos += _vel * dt;
    _rot += _rot_vel * dt;
}

