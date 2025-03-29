#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import dotz;
import vee;
import voo;
import vapp;

static constexpr const auto max_vertices = 256;

struct vtx {
  dotz::vec3 pos;
};

static unsigned g_count {};
static void map_buf(voo::h2l_buffer & buf) {
  voo::memiter<vtx> m { buf.host_memory(), &g_count };
  for (auto i = 0; i < 5; i++) {
    float s = i - 5;
    float e = s + 4;
    m += { .pos { e, e, -20.f + i } };
    m += { .pos { s, e, -20.f + i } };
    m += { .pos { s, s, -20.f + i } };

    m += { .pos { e, e, -20.f + i } };
    m += { .pos { s, s, -20.f + i } };
    m += { .pos { e, s, -20.f + i } };
  }
}

struct app : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::h2l_buffer buf { dq.physical_device(), sizeof(vtx) * max_vertices };
      map_buf(buf);

      auto pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        .back_face_cull = false,
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec3(0, 0),
        },
      });

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          buf.setup_copy(*pcb);

          auto scb = sw.cmd_render_pass({
            .command_buffer = *pcb,
            .clear_colours { vee::clear_colour(0.01, 0.02, 0.03, 1.0) },
          });
          vee::cmd_set_viewport(*pcb, sw.extent());
          vee::cmd_set_scissor(*pcb, sw.extent());
          vee::cmd_bind_vertex_buffers(*pcb, 0, buf.local_buffer());
          vee::cmd_bind_gr_pipeline(*pcb, *gp);
          vee::cmd_draw(*pcb, g_count);
        });
      });
    });
  }
} t;
