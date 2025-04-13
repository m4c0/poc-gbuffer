#pragma leco app
#pragma leco add_shader "poc.1.vert"
#pragma leco add_shader "poc.1.frag"
#pragma leco add_resource "PavingStones138_1K-JPG_AmbientOcclusion.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Color.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Displacement.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_NormalDX.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_NormalGL.jpg"
#pragma leco add_resource "PavingStones138_1K-JPG_Roughness.jpg"

import dotz;
import jute;
import sires;
import sitime;
import traits;
import vee;
import voo;
import vapp;
import wavefront;

struct upc {
  float aspect;
  float time;
};

struct vtx {
  dotz::vec3 pos;
  dotz::vec2 uv;
  dotz::vec3 nrm;
  dotz::vec3 btgt;
  dotz::vec3 tgt;
};

static constexpr const auto vtx_count = 18;
static auto load_cube(vee::physical_device pd) {
  voo::h2l_buffer buf { pd, sizeof(vtx) * vtx_count };
  voo::memiter<vtx> m { buf.host_memory() };

  m += { .pos {  1,  1, 1 }, .uv { 1, 1 }, .nrm { 0, 0, 1 }, .btgt { 0, 1, 0 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, -1, 1 }, .uv { 0, 0 }, .nrm { 0, 0, 1 }, .btgt { 0, 1, 0 }, .tgt { 1, 0, 0 } };
  m += { .pos {  1, -1, 1 }, .uv { 1, 0 }, .nrm { 0, 0, 1 }, .btgt { 0, 1, 0 }, .tgt { 1, 0, 0 } };

  m += { .pos {  1,  1, 1 }, .uv { 1, 1 }, .nrm { 0, 0, 1 }, .btgt { 0, 1, 0 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1,  1, 1 }, .uv { 0, 1 }, .nrm { 0, 0, 1 }, .btgt { 0, 1, 0 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, -1, 1 }, .uv { 0, 0 }, .nrm { 0, 0, 1 }, .btgt { 0, 1, 0 }, .tgt { 1, 0, 0 } };

  m += { .pos {  1, 1,  1 }, .uv { 1, 1 }, .nrm { 0, 1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos {  1, 1, -1 }, .uv { 1, 0 }, .nrm { 0, 1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, 1, -1 }, .uv { 0, 0 }, .nrm { 0, 1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
                                                                            
  m += { .pos {  1, 1,  1 }, .uv { 1, 1 }, .nrm { 0, 1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, 1, -1 }, .uv { 0, 0 }, .nrm { 0, 1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, 1,  1 }, .uv { 0, 1 }, .nrm { 0, 1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };

  m += { .pos {  1, -1,  1 }, .uv { 1, 1 }, .nrm { 0, -1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, -1, -1 }, .uv { 0, 0 }, .nrm { 0, -1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos {  1, -1, -1 }, .uv { 1, 0 }, .nrm { 0, -1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };

  m += { .pos {  1, -1,  1 }, .uv { 1, 1 }, .nrm { 0, -1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, -1,  1 }, .uv { 0, 1 }, .nrm { 0, -1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };
  m += { .pos { -1, -1, -1 }, .uv { 0, 0 }, .nrm { 0, -1, 0 }, .btgt { 0, 0, 1 }, .tgt { 1, 0, 0 } };

  return buf;
}

struct app : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq) {
      auto vbuf = load_cube(dq.physical_device());

      constexpr const auto vec_fmt = VK_FORMAT_R32G32B32A32_SFLOAT;

      auto vec_att = vee::create_colour_attachment({
        .format = vec_fmt,
        .store_op = vee::attachment_store_op_dont_care,
        .final_layout = vee::image_layout_color_attachment_optimal,
      });
      auto rp = vee::create_render_pass({
        .attachments {{
          vee::create_colour_attachment(dq.physical_device(), dq.surface()),
          vec_att,
          vec_att,
          vee::create_depth_attachment(),
        }},
        .subpasses {{
          vee::create_subpass({
            .colours {{
              create_attachment_ref(0, vee::image_layout_color_attachment_optimal),
              create_attachment_ref(1, vee::image_layout_color_attachment_optimal),
              create_attachment_ref(2, vee::image_layout_color_attachment_optimal),
            }},
            .depth_stencil = create_attachment_ref(3, vee::image_layout_depth_stencil_attachment_optimal),
          }),
          vee::create_subpass({
            .colours {{
              create_attachment_ref(0, vee::image_layout_color_attachment_optimal),
              create_attachment_ref(1, vee::image_layout_color_attachment_optimal),
              create_attachment_ref(2, vee::image_layout_color_attachment_optimal),
            }},
          }),
        }},
        .dependencies {{
          vee::create_dependency({
            .src_subpass = 0,
            .src_stage_mask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dst_subpass = 1,
            .dst_stage_mask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dst_access_mask = VK_ACCESS_SHADER_READ_BIT,
          }),
        }},
      });

      voo::offscreen::colour_buffer pos_buf { dq.physical_device(), voo::extent_of(dq), vec_fmt };
      voo::offscreen::colour_buffer nrm_buf { dq.physical_device(), voo::extent_of(dq), vec_fmt };
      voo::swapchain_and_stuff sw { dq, *rp, {{
        pos_buf.image_view(),
        nrm_buf.image_view(),
      }} };

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
        .render_pass = *rp,
        .blends {
          vee::colour_blend_classic(),
          vee::colour_blend_none(),
          vee::colour_blend_none(),
        },
        .shaders {
          voo::shader("poc.1.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.1.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::uv)),
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::nrm)),
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::tgt)),
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::btgt)),
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

      sitime::stopwatch time {};
      upc pc {};
      bool loaded = false;
      extent_loop(dq.queue(), sw, [&] {
        pc.aspect = sw.aspect();
        pc.time = time.millis() / 1000.0f;
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          if (!loaded) {
            vbuf.setup_copy(*pcb);
            img_occ.setup_copy(*pcb);
            img_clr.setup_copy(*pcb);
            img_dsp.setup_copy(*pcb);
            img_ndx.setup_copy(*pcb);
            img_ngl.setup_copy(*pcb);
            img_rgh.setup_copy(*pcb);
          }

          voo::cmd_render_pass rpg {{
            .command_buffer = *pcb,
            .render_pass = *rp,
            .framebuffer = sw.framebuffer(),
            .extent = sw.extent(),
            .clear_colours {
              vee::clear_colour(0.01, 0.02, 0.03, 1.0),
              vee::clear_colour(0, 0, 0, 0),
              vee::clear_colour(0, 0, 0, 0),
            },
          }};
          vee::cmd_set_viewport(*pcb, sw.extent());
          vee::cmd_set_scissor(*pcb, sw.extent());
          vee::cmd_bind_vertex_buffers(*pcb, 0, vbuf.local_buffer());
          vee::cmd_push_vert_frag_constants(*pcb, *pl, &pc);
          vee::cmd_bind_descriptor_set(*pcb, *pl, 0, dset);
          vee::cmd_bind_gr_pipeline(*pcb, *gp);
          vee::cmd_draw(*pcb, vtx_count);
          vee::cmd_next_subpass(*pcb);
        });
      });
    });
  }
} t;
