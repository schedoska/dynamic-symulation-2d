#include "body_handler.h"
#include "body_handler.h"
#include "ds2/Utils.h"

body_handler::body_handler()
{
	_active = false;

	for (auto& i : _stretchers) {
		i.setFillColor(sf::Color::Black);
		i.setRadius(10);
		i.setOrigin(10, 10);
	}
	_border.setFillColor(sf::Color::Transparent);
	_border.setOutlineColor(sf::Color::Red);
	_border.setOutlineThickness(3);

	_rotator.setFillColor(sf::Color::Black);
	_rotator.setRadius(10);
	_rotator.setOrigin(10, 10);
}

void body_handler::update(const sf::Window* window)
{
	bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window));

	if (!clicked && _active) {
		_active = false;
	}
	else if (_active) {
		update_active(mouse_pos);
		update_handlers_pos();
		set_target();
		return;
	}

	for (int i = handler::top; i != handler::rotator; ++i) {
		handler h = static_cast<handler>(i);
		bool contains = _stretchers[i].getGlobalBounds().contains(mouse_pos);
		if (contains && !_active) {
			_current_handler = h;
			_active = true;
		}
	}
	bool contains_rotator = _rotator.getGlobalBounds().contains(mouse_pos);
	if (contains_rotator && !_active) {
		_current_handler = handler::rotator;
		_active = true;
	}
	if (borders_contains(mouse_pos) && !_active) {
		_current_handler = handler::position;
		_active = true;
		_grab_pos = mouse_pos - _border.getPosition();
	}
	update_handlers_pos();
}

void body_handler::draw(sf::RenderWindow* window)
{
	window->draw(_border);
	window->draw(_rotator);
	for (const auto& i : _stretchers) window->draw(i);
}

void body_handler::set_target(body* target)
{
	_target = target;
	_border.setPosition(500, 500);
	_border.setSize(sf::Vector2f(200,100));
	_border.setOrigin(100, 50);
	_border.setRotation(15);
}

void body_handler::update_handlers_pos()
{
	double w2 = _border.getSize().x / 2.0;
	double h2 = _border.getSize().y / 2.0;
	double r = 10;

	_stretchers[handler::top].setOrigin(r, h2 + r);
	_stretchers[handler::right].setOrigin(-w2 + r, r);
	_stretchers[handler::bottom].setOrigin(r, -h2 + r);
	_stretchers[handler::left].setOrigin(w2 + r, r);

	_stretchers[handler::top_left].setOrigin(w2 + r, h2 + r);
	_stretchers[handler::top_right].setOrigin(-w2 + r, h2 + r);
	_stretchers[handler::bottom_right].setOrigin(-w2 + r, -h2 + r);
	_stretchers[handler::bottom_left].setOrigin(w2 + r, -h2 + r);

	for (auto& i : _stretchers) {
		i.setPosition(_border.getPosition());
		i.setRotation(_border.getRotation());
	}

	sf::Vector2f size = _border.getSize();
	_border.setOrigin(size.x / 2.0, size.y / 2.0);

	_rotator.setOrigin(r, h2 + 40 + r);
	_rotator.setPosition(_border.getPosition());
	_rotator.setRotation(_border.getRotation());
}

void body_handler::update_active(const sf::Vector2f& mouse_pos)
{
	if (_current_handler == handler::position) {
		_border.setPosition(mouse_pos - _grab_pos);
		return;
	}
	else if (_current_handler == handler::rotator) {
		vl::vec2d dv = utils::sfml_to_vec2d(mouse_pos) - utils::sfml_to_vec2d(_border.getPosition());
		vl::vec2d ref = { 0, -1 };
		double ang = utils::angle(ref, dv);
		ang = snap_angle(ang, 8);
		_border.setRotation(utils::RadToDegrees(ang));
	}

	sf::Vector2f loc_mouse_pos = mouse_pos - _border.getPosition();
	vl::vec2d lmpv = utils::sfml_to_vec2d(loc_mouse_pos);
	vl::vec2d hnv = vl::rotate(vl::vec2d(0, 1), utils::DegreesToRad(_border.getRotation()));
	vl::vec2d wnv = vl::rotate(vl::vec2d(1, 0), utils::DegreesToRad(_border.getRotation()));
	double h_val = hnv.dot(lmpv);
	double w_val = wnv.dot(lmpv);

	auto stretch = [=](handler h, const sf::Vector2f& v) {
		_stretchers[h].setOrigin(v);
		stretch_border(h);
	};

	constexpr double min_b = 20;	// minimal border size dimension
	constexpr double r = 10;		// radius of stretrcher handler
	switch (_current_handler)
	{
	case handler::top:
		stretch(handler::top, sf::Vector2f(10, -std::min(h_val, -min_b) + r));
		break;
	case handler::bottom:
		stretch(handler::bottom, sf::Vector2f(10, -std::max(h_val, min_b) + r));
		break;
	case handler::right:
		stretch(handler::right, sf::Vector2f(-std::max(w_val, min_b) + r, r));
		break;
	case handler::left:
		stretch(handler::left, sf::Vector2f(-std::min(w_val, -min_b) + r, r));
		break;
	case handler::top_left:
		stretch(handler::top, sf::Vector2f(10, -std::min(h_val, -min_b) + r));
		stretch(handler::left, sf::Vector2f(-std::min(w_val, -min_b) + r, r));
		break;
	case handler::top_right:
		stretch(handler::top, sf::Vector2f(10, -std::min(h_val, -min_b) + r));
		stretch(handler::right, sf::Vector2f(-std::max(w_val, min_b) + r, r));
		break;
	case handler::bottom_right:
		stretch(handler::bottom, sf::Vector2f(10, -std::max(h_val, min_b) + r));
		stretch(handler::right, sf::Vector2f(-std::max(w_val, min_b) + r, r));
		break;
	case handler::bottom_left:
		stretch(handler::bottom, sf::Vector2f(10, -std::max(h_val, min_b) + r));
		stretch(handler::left, sf::Vector2f(-std::min(w_val, -min_b) + r, r));
		break;
	}
}

void body_handler::stretch_border(handler h)
{
	sf::Vector2f hv = _stretchers[handler::top].getOrigin() - _stretchers[handler::bottom].getOrigin();
	sf::Vector2f wv = _stretchers[handler::right].getOrigin() - _stretchers[handler::left].getOrigin();
	sf::Vector2f size_new = sf::Vector2f(utils::sfml_to_vec2d(wv).len(), utils::sfml_to_vec2d(hv).len());

	sf::Vector2f size_diff = size_new - _border.getSize();
	_border.setSize(size_new);

	sf::Vector2f sdr = utils::rotate(size_diff, 0.0174532925 * _border.getRotation());
	if (h == handler::top || h == handler::left) {
		_border.setPosition(_border.getPosition() - (float)0.5 * sdr);
	}
	else {
		_border.setPosition(_border.getPosition() + (float)0.5 * sdr);
	}
}

bool body_handler::borders_contains(const sf::Vector2f& v)
{
	vl::vec2d origin_offset = { 10,10 };

	auto get_vec2d = [=](handler h) {
		sf::Vector2f v = _stretchers[h].getGlobalBounds().getPosition();
		return utils::sfml_to_vec2d(v);
	};
	vl::vec2d vs2d[4] = { 
		get_vec2d(handler::top_left) + origin_offset, 
		get_vec2d(handler::top_right) + origin_offset,
		get_vec2d(handler::bottom_right) + origin_offset,
		get_vec2d(handler::bottom_left) + origin_offset
	};
	vl::vec2d v2d = utils::sfml_to_vec2d(v);

	for (size_t i = 0; i < 4; ++i) {
		vl::vec2d dp = vs2d[(i + 1) % 4] - vs2d[i];
		vl::vec2d dv = v2d - vs2d[i];
		double s = vl::cross(dp, dv);
		if (s < 0) return false;
	}
	return true;
}

double body_handler::snap_angle(const double& rad, const int n)
{
	const double m_pi = acos(-1);
	double step = (2.0 * m_pi / n);
	double norm = (2.0 * rad + step) / (4.0 * m_pi);
	return (int)(norm * n) * step;
}

void body_handler::set_target()
{
	ds2::rect tb = _target->shape().box(_target->pos(), 0);
	sf::Vector2f b_size = _border.getSize();
	vl::vec2d s = { b_size.x / tb.w, b_size.y / tb.h };
	_target->shape().scale(s);
	_target->rot() = utils::DegreesToRad(_border.getRotation());

	vl::vec2d gc = tb.pos + vl::vec2d(tb.w, tb.h) * 0.5;
	vl::vec2d gcmc = gc - _target->pos();
	gcmc = vl::rotate(gcmc, _target->rot());
	_target->pos() = utils::sfml_to_vec2d(_border.getPosition()) - gcmc;

	_target->update_shape();
}

