#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"
#pragma leco add_resource "PavingStones138_1K-JPG_AmbientOcclusion.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Color.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Displacement.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_NormalDX.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_NormalGL.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Roughness.jpg"
#pragma leco add_resource "model.obj"

import dotz;
import jute;
import sires;
import traits;
import vee;
import voo;
import vapp;
import wavefront;

using wavefront::vtx;

struct upc {
  dotz::vec3 norm;
};

struct btn {
  dotz::vec3 btgt;
  dotz::vec3 tgt;
};

struct app : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      auto [vbuf, vcount] = wavefront::load_model(dq.physical_device(), sires::real_path_name("model.obj"));

      voo::h2l_buffer btn_buf { dq.physical_device(), static_cast<unsigned>(sizeof(btn) * vcount) };
      {
        voo::memiter<vtx> v { vbuf.host_memory(), nullptr };
        voo::memiter<btn> m { btn_buf.host_memory(), nullptr };
        for (auto i = 0; i < vcount; i += 3) {
          for (auto j = 0; j < 3; j++) {
            auto k = i + j;
            auto k1 = i + (j + 1) % 3;
            auto k2 = i + (j + 2) % 3;
            auto e1 = v[k1].pos - v[k].pos;
            auto e2 = v[k2].pos - v[k].pos;
            auto u1 = v[k1].txt - v[k].txt;
            auto u2 = v[k2].txt - v[k].txt;
            float f = u1.x * u2.y - u1.y * u2.x;
            m[k].tgt  = (u2.y * e1 - u1.y * e2) / f;
            m[k].btgt = (-u2.x * e1 + u1.x * e2) / f;
          }
        }
      }

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
      auto pl = vee::create_pipeline_layout({
        *dsl,
      }, {
        vee::vert_frag_push_constant_range<upc>(),
      });
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
          vee::vertex_input_bind(sizeof(btn)),
        },
        .attributes {
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::txt)),
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::nrm)),
          vee::vertex_attribute_vec3(1, traits::offset_of(&btn::tgt)),
          vee::vertex_attribute_vec3(1, traits::offset_of(&btn::btgt)),
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

      upc pc { { 1, 1, 5 } };
      bool loaded = false;
      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          if (!loaded) {
            vbuf.setup_copy(*pcb);
            btn_buf.setup_copy(*pcb);
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
          vee::cmd_bind_vertex_buffers(*pcb, 0, vbuf.local_buffer());
          vee::cmd_bind_vertex_buffers(*pcb, 1, btn_buf.local_buffer());
          vee::cmd_push_vert_frag_constants(*pcb, *pl, &pc);
          vee::cmd_bind_descriptor_set(*pcb, *pl, 0, dset);
          vee::cmd_bind_gr_pipeline(*pcb, *gp);
          vee::cmd_draw(*pcb, vcount);
        });
      });
    });
  }
} t;
