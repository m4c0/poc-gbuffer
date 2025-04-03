#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"
#pragma leco add_resource "PavingStones138_1K-JPG_AmbientOcclusion.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Color.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Displacement.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_NormalDX.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_NormalGL.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Roughness.jpg"

import dotz;
import jute;
import traits;
import vee;
import voo;
import vapp;

static constexpr const auto max_vertices = 256;

struct vtx {
  dotz::vec3 pos;
  dotz::vec2 uv;
};

static unsigned g_count {};
static void map_buf(voo::h2l_buffer & buf) {
  voo::memiter<vtx> m { buf.host_memory(), &g_count };
  float dist = -10.f;
  for (auto i = 0; i < 5; i++) {
    float s = i - 5;
    float e = s + 4;
    m += { .pos { e, e, dist + i }, .uv { 1, 1 } };
    m += { .pos { s, e, dist + i }, .uv { 0, 1 } };
    m += { .pos { s, s, dist + i }, .uv { 0, 0 } };

    m += { .pos { e, e, dist + i }, .uv { 1, 1 } };
    m += { .pos { s, s, dist + i }, .uv { 0, 0 } };
    m += { .pos { e, s, dist + i }, .uv { 1, 0 } };
  }
}

struct app : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::h2l_buffer buf { dq.physical_device(), sizeof(vtx) * max_vertices };
      map_buf(buf);

      const auto load_image = [&](jute::view name) {
        return voo::load_sires_image(name, dq.physical_device());
      };

      auto img_occ = load_image("PavingStones138_1K-JPG_AmbientOcclusion.jpg");
      auto img_clr = load_image("PavingStones138_1K-JPG_Color.jpg");
      auto img_dsp = load_image("PavingStones138_1K-JPG_Displacement.jpg");
      auto img_ndx = load_image("PavingStones138_1K-JPG_NormalDX.jpg");
      auto img_ngl = load_image("PavingStones138_1K-JPG_NormalGL.jpg");
      auto img_rgh = load_image("PavingStones138_1K-JPG_Roughness.jpg");

      // TODO: setup multiple outputs
      // TODO: setup multiple subpasses

      auto dsl = vee::create_descriptor_set_layout({
        vee::dsl_fragment_sampler(),
        vee::dsl_fragment_sampler(),
        vee::dsl_fragment_sampler(),
        vee::dsl_fragment_sampler(),
        vee::dsl_fragment_sampler(),
        vee::dsl_fragment_sampler(),
      });
      auto pl = vee::create_pipeline_layout({ *dsl });
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
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::uv)),
        },
      });

      auto smp = vee::create_sampler(vee::linear_sampler);
      auto dpool = vee::create_descriptor_pool(1, {
        vee::combined_image_sampler(6)
      });
      auto dset = vee::allocate_descriptor_set(*dpool, *dsl);
      vee::update_descriptor_set(dset, 0, img_occ.iv(), *smp);
      vee::update_descriptor_set(dset, 1, img_clr.iv(), *smp);
      vee::update_descriptor_set(dset, 2, img_dsp.iv(), *smp);
      vee::update_descriptor_set(dset, 3, img_ndx.iv(), *smp);
      vee::update_descriptor_set(dset, 4, img_ngl.iv(), *smp);
      vee::update_descriptor_set(dset, 5, img_rgh.iv(), *smp);

      bool loaded = false;
      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          if (!loaded) {
            buf.setup_copy(*pcb);
            img_occ.setup_copy(*pcb);
            img_clr.setup_copy(*pcb);
            img_dsp.setup_copy(*pcb);
            img_ndx.setup_copy(*pcb);
            img_ngl.setup_copy(*pcb);
            img_rgh.setup_copy(*pcb);
          }

          auto scb = sw.cmd_render_pass({
            .command_buffer = *pcb,
            .clear_colours { vee::clear_colour(0.01, 0.02, 0.03, 1.0) },
          });
          vee::cmd_set_viewport(*pcb, sw.extent());
          vee::cmd_set_scissor(*pcb, sw.extent());
          vee::cmd_bind_vertex_buffers(*pcb, 0, buf.local_buffer());
          vee::cmd_bind_descriptor_set(*pcb, *pl, 0, dset);
          vee::cmd_bind_gr_pipeline(*pcb, *gp);
          vee::cmd_draw(*pcb, g_count);
        });
      });
    });
  }
} t;
