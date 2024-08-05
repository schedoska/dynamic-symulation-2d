#include "object.h"
#include "mat22.h"
#include "Utils.h"

ds2::object::object(){
    init();
}

ds2::object::object(const vl::vec2d& pos) {
    init();
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

double& ds2::object::inertia()
{
    return _inertia;
}

const double& ds2::object::inertia() const
{
    return _inertia;
}

ds2::shape_group& ds2::object::shape()
{
    return _shape;
}

const ds2::shape_group& ds2::object::shape() const
{
    return _shape;
}

void ds2::object::update(const double& dt)
{
    _pos += _vel * dt;
    _rot += _rot_vel * dt;
}

vl::vec2d ds2::object::global(const vl::vec2d& local)
{
    double x = cos(_rot) * local[0] - sin(_rot) * local[1] + _pos[0];
    double y = sin(_rot) * local[0] + cos(_rot) * local[1] + _pos[1];
    return vl::vec2d(x, y);
}

vl::vec2d ds2::object::local(const vl::vec2d& global)
{
    vl::vec2d v = global - pos();
    double x = cos(-_rot) * v[0] - sin(-_rot) * v[1];
    double y = sin(-_rot) * v[0] + cos(-_rot) * v[1];
    return vl::vec2d(x, y);
}

void ds2::object::apply_force(const vl::vec2d& force, const vl::vec2d& point, const double& dt)
{
    _vel += (force / _mass) * dt;
    _rot_vel += (vl::cross(point, local(force) + pos()) / _inertia) * dt;
    //_rot_vel += (vl::cross(point, utils::rotate(force, _rot)) / inertia) * dt;
    //std::cout << (vl::cross(point, local(force) + pos()) / inertia) * dt << "\n";
}

void ds2::object::adjust_inertia()
{
    double sh_j = _shape.second_moment_area() / _shape.area();
    _inertia = sh_j * _mass;
}

void ds2::object::set_mass(const double& mass, bool _adjust_inertia)
{
    _mass = mass;
    if (_adjust_inertia) adjust_inertia();
}

void ds2::object::set_density(const double& den, bool _adjust_inertia)
{
    _mass = den * _shape.area();
    if (_adjust_inertia) adjust_inertia();
}

void ds2::object::init()
{
    _pos = vl::vec2d();
    _mass = 1;
    _inertia = 1;
}
