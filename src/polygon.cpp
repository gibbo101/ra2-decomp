// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "polygon.h"
#include "swap.h"
#include <cmath>

// FUNCTION: GAMEMD 0x0063F080
void Rasterize_Polygon(const PolygonShapeStruct & shape, PolygonRasterStruct & raster)
{
	Point2D * out_points;
	const Point2D * vertices = shape.Vertices;
	const int &vertex_count = shape.Count;

	if (vertex_count) {
		int min_vertex = 0;
		int max_vertex = 0;
		int min_y = vertices[0].Y;
		int max_y = min_y;

		for (int i = 1; i < vertex_count; i++) {
			if (vertices[i].Y < min_y) {
				min_vertex = i;
				min_y = vertices[i].Y;
			} else if (vertices[i].Y > max_y) {
				max_vertex = i;
				max_y = vertices[i].Y;
			}
		}

		if (min_y != max_y) {
			int cur_vertex = min_vertex;

			while (vertices[cur_vertex].Y == min_y) {
				cur_vertex = (cur_vertex + 1) % vertex_count;
			}
			int right_chain_start = (vertex_count + cur_vertex - 1) % vertex_count;

			while (vertices[min_vertex].Y == min_y) {
				min_vertex = (vertex_count + min_vertex - 1) % vertex_count;
			}
			int left_chain_start = (min_vertex + 1) % vertex_count;

			int winding_direction = -1;
			int right_x = vertices[right_chain_start].X;
			int left_x = vertices[left_chain_start].X;

			int bottom_span_exists = left_x != right_x;
			if (bottom_span_exists == 1) {
				if (left_x > right_x) {
					winding_direction = bottom_span_exists;
					swap(left_chain_start, right_chain_start);
				}
			} else {
				int next_right = (right_chain_start + 1) % vertex_count;
				int prev_left = (vertex_count + left_chain_start - 1) % vertex_count;
				int test_y = vertices[left_chain_start].Y;

				if ((vertices[next_right].X - left_x) * (vertices[prev_left].Y - test_y) - (vertices[prev_left].X - left_x) * (vertices[next_right].Y - test_y) < 0) {
					winding_direction = 1;
					swap(left_chain_start, right_chain_start);
				}
			}

			int raster_point_count = bottom_span_exists - min_y + max_y - 1;
			raster.Count = raster_point_count;

			if (raster_point_count > 0) {
				raster.BaseY = min_y - bottom_span_exists + 1;
				if (raster.Points == NULL) {
					raster.Points = (Point2D*)new Point2DStruct[raster_point_count + 200];
				}
				if (raster.Points == NULL) {
					raster.Count = 0;
				} else {
					out_points = raster.Points;
					int skip_first = bottom_span_exists == 0;
					int prev_index = left_chain_start;
					int skip = skip_first;

					do {
						if (winding_direction > 0) {
							left_chain_start = (left_chain_start + 1) % vertex_count;
						} else {
							left_chain_start = (left_chain_start + vertex_count - 1) % vertex_count;
						}
						Rasterize_Line_Span(vertices[prev_index].X, vertices[prev_index].Y, vertices[left_chain_start].X, vertices[left_chain_start].Y, 1, skip_first, out_points);
						prev_index = left_chain_start;
						skip_first = 0;
					} while (prev_index != max_vertex);

					out_points = raster.Points;
					skip_first = skip;
					int direction = -winding_direction;
					int cur_index = right_chain_start;
					int next_index = right_chain_start;

					do {
						if (-winding_direction > 0) {
							next_index = (next_index + 1) % vertex_count;
						} else {
							next_index = (next_index + vertex_count - 1) % vertex_count;
							direction = -winding_direction;
						}
						Rasterize_Line_Span(vertices[cur_index].X - 1, vertices[cur_index].Y, vertices[next_index].X - 1, vertices[next_index].Y, 0, skip_first, out_points);
						cur_index = next_index;
						skip_first = 0;
					} while (cur_index != max_vertex);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0063F2E0
void Rasterize_Line_Span(int x0, int y0, int x1, int y1, int axis, int skip, Point2D *&points)
{
	Point2D *out = points;
	int xinc = (x1 - x0) > 0 ? 1 : -1;
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (dy > 0) {
		int abs_dx = abs(dx);

		if (abs_dx == 0) {
			int count = dy - skip;
			while (count-- > 0) {
				if (axis == 1)
					out->X = x0;
				else
					out->Y = x0;
				out++;
			}

		} else if (dy == abs_dx) {
			if (skip != 0)
				x0 += xinc;

			int count = dy - skip;
			while (count-- > 0) {
				if (axis == 1)
					out->X = x0;
				else
					out->Y = x0;
				x0 += xinc;
				out++;
			}

		} else if (dy > abs_dx) {
			int error = (dx >= 0) ? 0 : 1 - dy;
			if (skip != 0) {
				error += abs_dx;
				if (error > 0) {
					x0 += xinc;
					error -= dy;
				}
			}

			int count = dy - skip;
			while (count-- > 0) {
				if (axis == 1)
					out->X = x0;
				else
					out->Y = x0;

				error += abs_dx;
				if (error > 0) {
					x0 += xinc;
					error -= dy;
				}
				out++;
			}

		} else {
			int error = abs_dx / dy;
			int xstep = error * xinc;
			int errinc = abs_dx % dy;

			error = (dx >= 0) ? 0 : 1 - dy;

			if (skip != 0) {
				error += errinc;
				x0 += xstep;
				if (error > 0) {
					x0 += xinc;
					error -= dy;
				}
			}

			int count = dy - skip;
			while (count-- > 0) {
				if (axis == 1)
					out->X = x0;
				else
					out->Y = x0;

				error += errinc;
				x0 += xstep;
				if (error > 0) {
					x0 += xinc;
					error -= dy;
				}
				out++;
			}
		}
		points = out;
	}
}
